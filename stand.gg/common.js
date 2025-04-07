function simplify_key(key)
{
	key = key.toLowerCase();
	if(key.substr(0,5)=="stand")
	{
		key=key.substr(5);
	}
	if(key.substr(0,1)=="-")
	{
		key=key.substr(1);
	}
	if(key.substr(0,9)=="activate-")
	{
		key=key.substr(9);
	}
	return key;
}

function time()
{
	return Math.floor(Date.now() / 1000);
}

function update_activation_key(key)
{
	if(localStorage.getItem("activation_key")==localStorage.getItem("web_key"))
	{
		localStorage.removeItem("web_key");
	}
	localStorage.setItem("activation_key", key);
}

function store_account_data(data)
{
	if("account_id" in data)
	{
		localStorage.setItem("account_id", data.account_id);
		localStorage.setItem("last_account_data_update", time()); // Only update timer on a full update
	}
	if("activation_key" in data)
	{
		update_activation_key(data.activation_key);
	}
	if("privilege" in data)
	{
		localStorage.setItem("account_privilege", data.privilege);
	}
	if("suspended_for" in data)
	{
		localStorage.setItem("account_suspended_for", data.suspended_for);
	}
	if("coins" in data)
	{
		localStorage.setItem("account_coins", data.coins);
	}
	if("created_quiz_success" in data)
	{
		localStorage.setItem("account_created_quiz_success", data.created_quiz_success);
	}
	if("interested" in data)
	{
		localStorage.setItem("account_interested", data.interested);
	}
}

function forget_account_data()
{
	localStorage.removeItem("activation_key");
	localStorage.removeItem("account_privilege");
	localStorage.removeItem("account_suspended_for");
	localStorage.removeItem("account_coins");
	localStorage.removeItem("account_vpn_expiry");
	localStorage.removeItem("account_shared");
	localStorage.removeItem("last_account_data_update");
}

function format_coins(coins)
{
	return (coins / 100).toFixed(2).split(".").join(",");
}

function copy_to_clipboard(text)
{
	var input = document.createElement("textarea");
	input.innerHTML = text;
	document.body.appendChild(input);
	input.select();
	document.execCommand("copy");
	document.body.removeChild(input);
}
