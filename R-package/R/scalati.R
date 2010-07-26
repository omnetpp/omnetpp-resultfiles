#
# (C) 2010 CSEM SA, Neuchâtel, Switzerland.
# Jérôme Rousselot <jerome.rousselot@csem.ch>
#

library(omnetpp)
library(lattice)
library(gdata)   # needed for drop.levels
library(gWidgets)
options("guiToolkit"="RGtk2")	# try also: tcltk

configuration <- ""

loadData <- function() {
	configuration <<- ginput("Please enter the configuration name to study:")
	testFile <- paste(configuration, "-0.sca", sep="")
	while(! file.exists(testFile)) {
  		configuration <<- ginput(paste("File ", testFile, 
		" not found.\n Please enter the configuration name to study:", sep=""))
		testFile <- paste(configuration, "-0.sca", sep="")
	}

	x <- loadDataset(paste(configuration, "-*.sca", sep="")) #,
	runattrs2 <- reshape(x$runattrs, idvar="runid", timevar="attrname", direction="wide")
	xs <- x$scalars
	xs$qname <- paste(xs$module, xs$name, sep="-")
	xs <- reshape(xs, idvar="runid", timevar="qname", direction="wide", drop=c("resultkey", "name", "module", "file"))
	d <-  merge(xs, runattrs2)
	d <- drop.levels(d)
	# convert all numeric columns to factors
#	for(col in names(d)) {
#		if(class(eval(substitute(d$col, list(col=col))))=="numeric") {
#			d[col] <- as.factor(eval(substitute(d$col, list(col=col))))
#		}
#	}

	listOfModules <- unique(x$scalars$module)
	listOfNames <- lapply(listOfModules, function(moduleName) { unique(x$scalars[x$scalars$module==moduleName,]$name) })
	listOfAttrs <- unique(x$runattrs$attrname)
	names(listOfNames) <- listOfModules
	list(d=d, listOfModules=listOfModules, listOfNames=listOfNames, listOfAttrs=listOfAttrs)
}

# 2. Define lattice related auxiliary functions
#    and main getFigure function.

gridPanel <- function(...) {
  panel.grid(h=-1, v=-1, col="grey")       
  panel.xyplot(...,)
}

yscale.components.log10 <- function(lim, ...) {
    ans <- yscale.components.default(lim = lim, ...)
    tick.at.major <- c(0.001, 0.01, 0.1, 1, 10, 100, 1000) # seq(0.1, 1, 0.1) # c(0.25, 0.5, 0.75, 1) # seq(0.2, 1, 0.2)
    ans$left$labels$labels <- c("10⁻³", "10⁻²", "10⁻¹", "1", "10", "100", "1000")#tick.at.major
    ans$left$labels$at <- log(tick.at.major, 10)
    ans$left$ticks$tck <- 1.5
    ans$left$ticks$at  <- log(tick.at.major, 10)
    ans
}

# First parameter gives the y field of the plot
# Second parameter gives the x field of the plot
# Third parameter is a list of all fields to be used as lattice panel parameters (strip)
# Fourth parameter declares the field to use to separate points on each panel
# the four next parameters are text descriptions of the respective fields
# (the last two are vectors)
getFigure <- function(d, yVarRef, xVarRef, zVarRefs, groupBy,
			xlab, ylab, zlab, grouplab) {

	if(length(zVarRefs) > 0) {
		# generate as many zi fields in formula as we have substitution fields
		# identified in zVarRefs
		stripFields <- sapply(1:length(zVarRefs), function(x) { sprintf("z%s", x) })
		names(zVarRefs) <- stripFields
		stripFieldsString <- paste(stripFields, collapse=" * ")
		toSubstitute <- as.formula(paste("y ~ x | ", stripFieldsString, sep=""))
		strip <- strip.custom(strip.names = TRUE, 
		       strip.levels = TRUE, 
		       style=3,
			var.name = zlab)
	} else {
		toSubstitute <- as.formula("y ~ x")
		strip <- FALSE
	}
	substitutionValues <- c(y=yVarRef, x=xVarRef, zVarRefs)
	# generate the formula
	formula <- as.formula(do.call("substitute", list(toSubstitute, substitutionValues))) 
	# convert strip fields to factors
	for(col in zVarRefs) {
		if(class(d[[as.character(col)]])=="numeric") {
			d[[as.character(col)]] <- as.factor(d[[as.character(col)]])
		}
	}

	# generate the figure
	textSize <- 1
	figure <- xyplot(formula,
	    data=d,
		groups=eval(groupBy),
	    type='p',
	    xlab = list(label=xlab, cex=textSize),
	    ylab = list(label=ylab, cex=textSize),
	    horizontal=FALSE, as.table = TRUE, 
	    panel=gridPanel,
#	    par.settings = simpleTheme(pch=c(15,16), cex=1, lwd=1),
	    par.strip.text=list(cex=textSize),
	    strip = strip,
	    auto.key = list(space="bottom", 
#			    text=grouplab, 
			    cex=textSize)
	#    scales = list(y=list(log = 10), cex=textSize),
	#    yscale.components = yscale.components.log10
	#    layout = c(2, 2, 2) # 2 columns, 2 lines, 2 pages
	)
	figure
}

startGUI <- function() {
	tmp <- loadData()
	d <- tmp$d
	listOfNames <- tmp$listOfNames
	listOfModules <- tmp$listOfModules
	listOfAttrs <- tmp$listOfAttrs
	rm(tmp)
	win = gwindow("R Lattice analysis module for Omnet++ 4.1 sca files", visible=FALSE)
	gtop = ggroup(horizontal = FALSE, cont=win) # main group

	formulaLabel = glabel(text = "y ~ x | z1 * z2", cont=gtop)
	#modulesTable <- gtable(as.data.frame(listOfModules), cont=g, expand = TRUE)
	#size(l) <- c(400,300)

	bigPanel = ggroup(horizontal=TRUE, cont=gtop)
	#varGroup = ggroup(horizontal = FALSE, cont=gtop)
	varGroup = glayout(cont=bigPanel)
	#plotPanel = ggraphics(cont=bigPanel, width=800, height=600)
	#addSpring(legendVarGroup)
	# Header
	varGroup[1, 3] <- glabel(text="Module", cont=varGroup)
	varGroup[1, 4] <- glabel(text="Name", cont=varGroup)
	varGroup[1, 5] <- glabel(text="Description", cont=varGroup)
	# Enable
	varGroup[4, 1] <- enableGroup <- gcheckbox(NULL, cont=varGroup)
	varGroup[5, 1] <- enableZ1 <- gcheckbox(NULL, cont=varGroup)
	varGroup[6, 1] <- enableZ2 <- gcheckbox(NULL, cont=varGroup)
	# Labels column
	varGroup[2, 2] <- glabel(text="Y value: ", cont=varGroup)
	varGroup[3, 2] <- glabel(text="X value: ", cont=varGroup)
	varGroup[4, 2] <- glabel(text="Group by: ", cont=varGroup)
	varGroup[5, 2] <- glabel(text="Z1 value: ", cont=varGroup)
	varGroup[6, 2] <- glabel(text="Z2 value: ", cont=varGroup)
	# Module names comboboxes column
	varGroup[2, 3] <- yModCombo <- gcombobox(listOfModules, cont=varGroup)
	varGroup[3, 3] <- xModCombo <- gcombobox(listOfModules, cont=varGroup)
#	varGroup[4, 3] <- groupModCombo <- gcombobox(listOfModules, cont=varGroup)
#	varGroup[5, 3] <- z1ModCombo <- gcombobox(listOfModules, cont=varGroup)
#	varGroup[6, 3] <- z2ModCombo <- gcombobox(listOfModules, cont=varGroup)
	# Module parameter names comboboxes column
	varGroup[2, 4] <- yNameCombo <- gcombobox(listOfNames[["."]], cont=varGroup)
	varGroup[3, 4] <- xNameCombo <- gcombobox(listOfNames[["."]], cont=varGroup)
#	varGroup[4, 4] <- groupNameCombo <- gcombobox(listOfNames[["."]], cont=varGroup)
#	varGroup[5, 4] <- z1NameCombo <- gcombobox(listOfNames[["."]], cont=varGroup)
#	varGroup[6, 4] <- z2NameCombo <- gcombobox(listOfNames[["."]], cont=varGroup)
	varGroup[4, 4] <- groupNameCombo <- gcombobox(listOfAttrs, cont=varGroup)
	varGroup[5, 4] <- z1NameCombo <- gcombobox(listOfAttrs, cont=varGroup)
	varGroup[6, 4] <- z2NameCombo <- gcombobox(listOfAttrs, cont=varGroup)

	# field descriptions (for plot annotation)
	varGroup[2, 5] <- yLegend <- gedit("")
	varGroup[3, 5] <- xLegend <- gedit("")
	varGroup[4, 5] <- groupLegend <- gedit("")
	varGroup[5, 5] <- z1Legend <- gedit("")
	varGroup[6, 5] <- z2Legend <- gedit("")
	enabled(groupLegend) <- FALSE
	# handlers
	addHandlerChanged(yModCombo, function(h,...) {yNameCombo[] <- listOfNames[[svalue(yModCombo)]]})
	addHandlerChanged(xModCombo, function(h,...) {xNameCombo[] <- listOfNames[[svalue(xModCombo)]]})
#	addHandlerChanged(groupModCombo, function(h,...) {groupNameCombo[] <- listOfNames[[svalue(groupModCombo)]]})
#	addHandlerChanged(z1ModCombo, function(h,...) {z1NameCombo[] <- listOfNames[[svalue(z1ModCombo)]]})
#	addHandlerChanged(z2ModCombo, function(h,...) {z2NameCombo[] <- listOfNames[[svalue(z2ModCombo)]]})

	buttonGroup <- ggroup(horizontal = TRUE, cont=gtop)
	plotWinButton <- gbutton("plot in a window", cont=buttonGroup)
	#addSpring(buttonGroup)
	plotPNGButton <- gbutton("plot to a PNG file", cont=buttonGroup)
	plotEPSButton <- gbutton("plot to an EPS file", cont=buttonGroup)
#	addSpace(buttonGroup, 10)
	addSpring(buttonGroup)
	exitButton <- gbutton("Exit", cont=buttonGroup, handler= function(h,...) { dispose(win) })

	getFigureFromGUI <- function(h,...) {
		yVarRef <- as.name(paste("value.", svalue(yModCombo), "-", svalue(yNameCombo), sep=""))
		xVarRef <- as.name(paste("value.", svalue(xModCombo), "-", svalue(xNameCombo), sep=""))
		if(svalue(enableGroup)) {
#			groupBy <- as.name(paste("value.", svalue(groupModCombo), "-", 
#				svalue(groupNameCombo), sep=""))
			groupBy <- as.name(paste("attrvalue.", svalue(groupNameCombo), sep=""))
		} else {
			groupBy <- NULL
		}
		if(svalue(enableZ1)) {
#			z1VarRef <- as.name(paste("value.", svalue(z1ModCombo), "-", 
#				svalue(z1NameCombo), sep=""))
			z1VarRef <- as.name(paste("attrvalue.", svalue(z1NameCombo), sep=""))

		} else {

			z1VarRef <- NULL
		}
		if(svalue(enableZ2)) {
#			z2VarRef <- as.name(paste("value.", svalue(z2ModCombo), "-", 
#				svalue(z2NameCombo), sep=""))
			z2VarRef <- as.name(paste("attrvalue.", svalue(z2NameCombo), sep=""))
		} else {
			z2VarRef <- NULL
		}
	#	zVarRefs <- c()
	#	for(i in 1:2) {
	#		zVarRefs <- c(zVarRefs, 
	#	}
		zVarRefs <- c(z1VarRef, z2VarRef)
		xlab <- svalue(xLegend)
		ylab <- svalue(yLegend)
		grouplab <- svalue(groupLegend)
		z1lab <- svalue(z1Legend)
		z2lab <- svalue(z2Legend)
		zlab <- c(z1lab, z2lab)
		figure <- getFigure(d, yVarRef, xVarRef, zVarRefs, groupBy,
				xlab=xlab,
				ylab=ylab,
				zlab=zlab,
				grouplab=grouplab)
		figure
	}

	plotX11FigureFromGUI <- function(h,...) {
		figure <- getFigureFromGUI(h,...)
		X11() 
		print(figure)
	}

	plotPNGFigureFromGUI <- function(h,...) {
		figure <- getFigureFromGUI(h,...)
		figureName <- ginput("Please enter a name without spaces for this figure. The files will be named as: configuration_name-%d.png", title="Graphical file output parameter")
		png(filename=paste(configuration, "_", figureName, "-%d.png", sep=""), width=800, height=600)
		print(figure)
		dev.off()
	}

	plotEPSFigureFromGUI <- function(h,...) {
		trellis.device(device=postscript, color=TRUE, theme=col.whitebg)
		figure <- getFigureFromGUI(h,...)
		figureName <- ginput("Please enter a name without spaces for this figure. The files will be named as: configuration_name-%d.eps", title="Graphical file output parameter")
		postscript(file=paste(configuration, "_", figureName, "-%d.eps", sep=""), 
			onefile=FALSE, horizontal=T, colormodel="cmyk")
		print(figure)
		dev.off()
	}

	addHandlerChanged(plotWinButton, plotX11FigureFromGUI)
	addHandlerChanged(plotPNGButton, plotPNGFigureFromGUI)
	addHandlerChanged(plotEPSButton, plotEPSFigureFromGUI)

	visible(win) <- TRUE
}

