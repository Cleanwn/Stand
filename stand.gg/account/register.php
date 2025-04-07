<!DOCTYPE html>
<html>
<head>
	<title>Create Account | Stand</title>
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link href="/halfmoon-variables.min.css" crossorigin="anonymous" rel="stylesheet" />
	<link href="/halfmoon-config.css" rel="stylesheet" />
</head>
<body class="dark-mode">
	<div class="page-wrapper with-navbar">
		<nav class="navbar">
			<div class="container-xl">
				<a class="navbar-brand" href="/">Stand</a>
				<ul class="navbar-nav">
					<li class="nav-item active"><a class="nav-link" href="/account/">Account</a></li>
					<li class="nav-item"><a class="nav-link" href="/help/">Help</a></li>
				</ul>
			</div>
		</nav>
		<div class="content-wrapper">
			<div class="container-xl">
				<div class="content">
					<h1>Create Stand Account</h1>
					<form onsubmit="submit_register();return false;" method="GET" action="/bad_browser.txt">
						<p><input type="text" class="form-control" placeholder="License key" required pattern="([a-z0-9]{30}|STAND[a-z0-9]{30,31}|Stand-[a-z0-9]{30,31}|[a-z0-9]{30,31}Stand-Activate-[a-z0-9]{30,31})" title="Stand License Key" /></p>
						<p><input type="submit" class="btn btn-primary" value="Register" /></p>
					</form>
					<p>Usage of Stand for GTA V is subject to our <a href="/help/terms-of-service" target="_blank">Terms of Service</a>.</p>
				</div>
				<div class="content">
					<h3 class="font-weight-lighter">Don't have a license key?</h3>
				</div>
				<div class="row pl-20 pr-20 mb-20">
					<div class="col-md-4">
						<div class="card p-20 m-10">
							<h2 class="card-title">Basic</h2>
							<p>For about <b>12,50€</b>, you get:</p>
							<?php require "../components/ed-basic.html"; ?>
							<td><a href="/help/basic-edition" class="btn btn-default">WHERE TO BUY</a></td>
						</div>
					</div>
					<div class="col-md-4">
						<div class="card p-20 m-10">
							<h2 class="card-title">Regular</h2>
							<p>For about <b>25€</b>, you get:</p>
							<?php require "../components/ed-regular.html"; ?>
							<td><a href="/help/regular-edition" class="btn btn-default">WHERE TO BUY</a></td>
						</div>
					</div>
					<div class="col-md-4">
						<div class="card p-20 m-10">
							<h2 class="card-title">Ultimate</h2>
							<p>For about <b>50€</b>, you get:</p>
							<?php require "../components/ed-ultimate.html"; ?>
							<td><a href="/help/ultimate-edition" class="btn btn-default">WHERE TO BUY</a></td>
						</div>
					</div>
				</div>
			</div>
		</div>
	</div>
	<script src="/common.js"></script>
	<script>
		if(typeof localStorage.getItem("account_id")=="string")
		{
			location.href=".";
		}

		let register_pending = false;
		function submit_register()
		{
			if(register_pending)
			{
				return;
			}
			register_pending = true;
			let license_key = simplify_key(document.querySelector("input[type='text']").value);
			if(license_key.length!=30)
			{
				if(license_key.length==31)
				{
					location.href="migrate";
				}
				return;
			}
			$.post("/api/redeem",{license_key}).done(function(data)
			{
				if(data.error)
				{
					alert(data.error);
					return;
				}
				store_account_data(data);
				location.href="created#"+data.account_id;
			}).fail(function()
			{
				alert("Failed to send register request. Reload the page and try again. Try a different browser if this issue persists.");
			}).always(function()
			{
				register_pending = false;
			});
		}
	</script>
	<script src="https://code.jquery.com/jquery-3.6.0.min.js" integrity="sha256-/xUj+3OJU5yExlq6GSYGSHk7tPXikynS7ogEvDej/m4=" crossorigin="anonymous"></script>
</body>
</html>
