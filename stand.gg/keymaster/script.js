if (localStorage.getItem("account_id") === null)
{
	location.href = "/account/";
}
else
{
	document.querySelectorAll("input[name='auth']").forEach(e => e.value = localStorage.getItem("account_id"));
}
