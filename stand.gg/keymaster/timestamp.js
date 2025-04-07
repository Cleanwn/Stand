(function()
{
	window.timestamp = {
		get: () => (new Date() / 1000),
		toDate: ts => new Date(ts+((new Date()).getTimezoneOffset()*-1/60*3600)),
		isSameDay: function(a, b)
		{
			return (a.getFullYear()==b.getFullYear() && a.getDate()==b.getDate() && a.getMonth()==b.getMonth());
		},

		strings: {
			year: "{y}-{m}-{d}",
			date: "{m}-{d}",
			time: "{h}:{i}",
			datetime: "{date} {time}",
		},
		format: function(date, str)
		{
			return str.split("{h}").join(justifyNum(date.getHours()))
			.split("{i}").join(justifyNum(date.getMinutes()))
			.split("{d}").join(justifyNum(date.getDate()))
			.split("{m}").join(justifyNum(date.getMonth()+1))
			.split("{y}").join(justifyNum(date.getFullYear()));
		},
	};

	function justifyNum(num)
	{
		if (num < 10)
		{
			num = "0" + num;
		}
		return num;
	}

	function updateElements()
	{
		let now=new Date();
		document.querySelectorAll("[data-time]:not([data-ignore-time-element])").forEach(elm => {
			let ts = (parseInt(elm.getAttribute("data-time"))*1000);
			let date = timestamp.toDate(ts);

			let showTime = !elm.hasAttribute("data-notime");
			let showDate = !showTime || !timestamp.isSameDay(now, date);

			const dateFmt = (now.getFullYear()==date.getFullYear() ? timestamp.strings.date : timestamp.strings.year);

			if (showDate && showTime)
			{
				elm.textContent = timestamp.strings.datetime
				.split("{date}").join(timestamp.format(date, dateFmt))
				.split("{time}").join(timestamp.format(date, timestamp.strings.time));
			}
			else if (showDate)
			{
				elm.textContent = timestamp.format(date, dateFmt);
			}
			else
			{
				elm.textContent = timestamp.format(date, timestamp.strings.time);
			}
		});
	}

	var observer = new MutationObserver(function(mutations)
	{
		disableObserver();
		updateElements();
		enableObserver();
	});

	function enableObserver()
	{
		observer.observe(document, {attributes: true, childList: true, characterData: true, subtree: true });
	}

	function disableObserver()
	{
		observer.disconnect();
	}

	updateElements();
	enableObserver();
})();
