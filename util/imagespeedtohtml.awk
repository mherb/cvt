BEGIN {
	printf("<html>\n<header>\n</header>\n<body><table>\n");
}

/^Image Convert Speed/ { printf("<tr><td colspan=2><h1>"$0"</h1></td></tr>\n"); }

/^[\t ]*Format:/ {printf("<tr>\n<td style=\"width:300px;\">%s to %s</td>\n<td><div style=\"-moz-border-radius: 8px; -webkit-border-radius: 8px; background-color: #00ff00;height: 16px; width: %dpx; border:1px solid gray; padding-left:10px; font-size: 12px;\">%.2f MP/s</div></td>\n</tr>\n",$2,$5,$7/2+30,$7);
}
END {
	printf("</table></body>\n</html>\n");
}
