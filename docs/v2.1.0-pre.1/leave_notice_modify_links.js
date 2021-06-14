$(document).ready(function(){
	var re_nist = new RegExp('https?:\/\/[^/]*.?nist.gov');
	var re_absolute_address = new RegExp('^(?!.*:\/\/).*');

	$("a").each(function() {
		var url = $(this).attr('href');
		if (re_nist.test(url) || re_absolute_address.test(url)) {
			$(this).addClass('local');
		} else {
			$(this).addClass('external');
		}
	});

	$('a.external').leaveNotice();
});

