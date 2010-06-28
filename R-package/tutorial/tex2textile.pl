#!/usr/bin/env perl
#
# Converts basic Latex to the Textile markup that the github wiki uses
#

$txt = "";
while (<>) {
    $txt .= $_;
}

# convert verbatim blocks
$txt =~ s/\\begin{verbatim}/bc../sg;
$txt =~ s/\\end{verbatim}/p./sg;

# convert section headers
$txt =~ s/\\section\{(.*?)\}/h1. $1/sg;
$txt =~ s/\\subsection\{(.*?)\}/h2. $1/sg;

# convert monospace font
$txt =~ s/\\ttt\{(.*?)\}/\@$1@/sg;

# remove remaining backslashes (\$, \%, \_)
$txt =~ s/\\([\$%_])/$1/sg;

print $txt;
