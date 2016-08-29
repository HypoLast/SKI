function eliminate(expr, variable) {
	var result = "";
	var reg = /`|[a-zA-Z]|(<[A-Z]+>)/g;
	var match;
	while(match = reg.exec(expr)) {
		var c = match[0];
		if (c == "`") result += "``S";
		else if (c == variable) result += "I";
		else result += "`K" + c;
	}
	return result;
}

module.exports = eliminate;