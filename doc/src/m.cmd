call c:\home\omnetpp40\omnetpp\setenv-vc90.bat
set PATH=%PATH%;c:\Latex\MiKTeX-2.8\miktex\bin


pdflatex main.tex
makeindex main
pdflatex main.tex
pdflatex main.tex
rm -f *.aux *.idx *.ilg *.ind *.log *.out *.toc *.blg

copy /y main.pdf ..\resultfiles.pdf