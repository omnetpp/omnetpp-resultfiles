#!/usr/bin/env perl
#
# Converts basic Latex to the Textile markup that the github wiki uses
#
# NOTE that gitgub wiki also understands HTML, only in a brain-damaged way:
# it keeps line break within <p> (!!!!!) and one MUST NOT quote ">" within
# <pre> blocks (!!!!!!). Better stick to Textile. Maybe less brain damage,
# although it badly breaks on reandering "h[[1]]" within a <pre> block.
#

$txt = "";
while (<>) {
    $txt .= $_;
}

# remove end-line spaces
$txt =~ s/ +\n/\n/sg;

# temporarily remove (encode) verbatim regions
$i = 0;
$txt =~ s|\\begin{verbatim}\n*(.*?)\n?\\end{verbatim}|$pre{++$i}=$1;"#PRE".$i."#"|sge;

# convert all paragraphs: remove newlines, prefix with "p."
$txt =~ s/([^\n])\n([^\n])/$1 $2/sg;
$txt =~ s/\n\n([^\n])/\n\np. $1/sg;

# convert monospace font
$txt =~ s/\\ttt\{(.*?[^\\])\}/\@$1@/sg;

# convert image
$txt =~ s|\\image\{(.*?)\}|!$1!|sg;

# convert section headings
$txt =~ s/p\. +\\section\{(.*?)\}/h1. $1/sg;
$txt =~ s/p\. +\\subsection\{(.*?)\}/h2. $1/sg;

# restore verbatim regions
$txt =~ s/p\. *#PRE([0-9]+)#/"bc.. " . $pre{$1}/sge;
$txt =~ s/\np\. h1\./\nh1./sg;

# remove some remaining backslashes (\$, \%, \_, \{, \})
$txt =~ s/\\([\$%_{}])/$1/sg;

# random fixes: textile misunderstands hh[[1]] in the tuturial, the only workaround seems to be
# to add a space in between the two open brackets (textile defines no escaping, 
# newlines make no difference, etc). This makes the code not valid R, but still better than
# the garbage that comes out of it otherwise...
$txt =~ s/\[\[/[ [/sg;

print $txt;

