#!/usr/bin/env perl
#
# Converts basic Latex to HTML
#

$txt = "";
while (<>) {
    $txt .= $_;
}

# quote angle brackets
$txt =~ s|<|&lt;|sg;
$txt =~ s|>|&gt;|sg;

# convert verbatim blocks
$txt =~ s|\\begin{verbatim}|<pre>|sg;
$txt =~ s|\\end{verbatim}|</pre>|sg;

# convert section headers
$txt =~ s|\\section\{(.*?)\}|<h1>$1</h1>|sg;
$txt =~ s|\\subsection\{(.*?)\}|<h2>$1</h2>|sg;

# convert monospace font
$txt =~ s|\\ttt\{(.*?)\}|<tt>$1</tt>|sg;

# add <p> (but not inside <pre> tags)
$i = 0;
$txt =~ s|(<pre>.*?</pre>)|$pre{++$i}=$1;"#PRE".$i."#"|sge;
$txt =~ s|\n\n|\n\n<p>\n|sg;
$txt =~ s|#PRE([0-9]+)#|$pre{$1}|sge;

# remove remaining backslashes (\$, \%, \_)
$txt =~ s|\\([\$%_])|$1|sg;

# remove redundant markup (<p><pre>)
$txt =~ s|<p>\n<pre>|<pre>|sg;

print $txt;
