<!DOCTYPE html>
<html>
<head>
	<title>Account Management | Stand</title>
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
					<li class="nav-item active"><a class="nav-link" href="#">Account</a></li>
					<li class="nav-item"><a class="nav-link" href="/help/">Help</a></li>
				</ul>
			</div>
		</nav>
		<div class="content-wrapper">
			<div class="container-xl">
				<div id="logged-out">
					<div class="content">
						<h1>Stand Account Management</h1>
						<p>Nice to meet you!</p>
						<p>
							<a href="register" class="btn btn-primary">Create Account</a>
							<a href="login" class="btn btn-default">Login</a>
						</p>
					</div>
				</div>
				<div id="logged-in" class="d-none">
					<div class="content">
						<h1>Stand Account Management</h1>
						<p>You're logged in with account id <code id="account-id"></code> <button id="reveal-account-id" class="btn btn-small btn-default">Reveal</button> <button onclick="logout();" class="btn btn-small btn-default">Logout</button></p>
						<div id="quiz-finisher" class="alert alert-secondary d-none" role="alert">You seem to have skipped the post-registration process. Please <b><a href="created-quiz" class="alert-link">finish it now</a></b> as we can not help you recover your account otherwise.</div>
					</div>
					<div class="row pl-20 pr-20">
						<div class="d-none">
							<div class="card p-20 m-10">
								<h3 class="card-title mb-0">If you can see this message, your shit is fucked.</h3>
							</div>
						</div>
						<div class="suspended-yes d-none">
							<div class="card p-20 m-10">
								<h3 class="card-title mb-0">This account has been suspended. Reason: <span id="suspended-for"></span>.</h3>
							</div>
						</div>
						<div class="privilege-1 privilege-2 privilege-3 d-none col-lg-6">
							<div class="card p-20 m-10">
								<h3 class="card-title">Activation Key</h3>
								<p>Copy this to activate Stand for GTA V.</p>
								<p><input id="activation-key" type="text" class="form-control" readonly /></p>
								<p>
									<a class="btn btn-primary" href="https://github.com/calamity-inc/Stand-Launchpad/releases/latest/download/Stand.Launchpad.exe" download>Download Launchpad</a>
									<button class="btn btn-default" onclick="regenerateActivationKey();">Regenerate Key</button>
								</p>
							</div>
						</div>
						<div class="suspended-no d-none col-lg-3 col-sm-6">
							<div class="uk-card uk-card-default uk-margin-bottom">
								<div class="card p-20 m-10">
									<h3 class="card-title">Need Help?</h3>
									<p>
										<a class="btn btn-primary" href="/help" target="_blank">Get Help With Stand</a>
									</p>
								</div>
							</div>
						</div>
						<div class="suspended-no d-none col-lg-3 col-sm-6">
							<div class="uk-card uk-card-default uk-margin-bottom">
								<div class="card p-20 m-10">
									<h3 class="card-title">Community Discord</h3>
									<p>
										<a class="btn btn-primary" id="discord-join-link" href="#">Join</a>
										<a class="btn btn-default" id="discord-update-link" href="#">Update Role</a>
									</p>
									<!--<p><a href="https://guilded.gg/stand" target="_blank">We're also on Guilded.</a></p>-->
								</div>
							</div>
						</div>
						<div class="have-coins d-none col-xl-3 col-sm-6">
							<div class="uk-card uk-card-default uk-margin-bottom">
								<div class="card p-20 m-10">
									<h3 class="card-title">Your Balance (<span id="balance"></span>€)</h3>
									<a class="btn btn-primary" href="shop">Visit Shop</a>
									<a class="btn btn-default" href="topup">Top Up</a>
								</div>
							</div>
						</div>
						<div class="privilege-2 privilege-3 d-none col-xl-3 col-sm-6">
							<div class="uk-card uk-card-default uk-margin-bottom">
								<div class="card p-20 m-10">
									<h3 class="card-title">Web Interface</h3>
									<p>
										<a id="web-interface-link" class="btn btn-primary" target="_blank">Open Web Interface</a>
									</p>
								</div>
							</div>
						</div>
						<div class="privilege-1 privilege-2 d-none col-sm-3">
							<div class="uk-card uk-card-default uk-margin-bottom">
								<div class="card p-20 m-10">
									<div class="privilege-1 d-none">
										<h3 class="card-title">You have Basic Edition</h3>
										<p>Consider upgrading to <b>Regular</b> for about 12,50€ to get:</p>
										<?php require "../components/ed-regular.html"; ?>
										<a class="btn btn-default" href="/help/basic-to-regular-edition" target="_blank">Learn More</a>
									</div>
									<div class="privilege-2 d-none">
										<h3 class="card-title">You have Regular Edition</h3>
										<p>Consider upgrading to <b>Ultimate</b> for about 25€ to get:</p>
										<?php require "../components/ed-ultimate.html"; ?>
										<a class="btn btn-default" href="/help/regular-to-ultimate-edition" target="_blank">Learn More</a>
									</div>
									<div class="privilege-1 privilege-2 d-none">
										<h4 class="mt-20 font-weight-lighter">Use your upgrade key</h4>
										<form onsubmit="submit_upgrade();return false;">
											<div class="input-group">
												<input id="upgrade-key" type="text" class="form-control" placeholder="Upgrade key" required pattern="Stand-Upgrade-[a-z0-9]{31}" title="Stand Upgrade Key" />
												<div class="input-group-append">
													<input type="submit" class="btn btn-primary" value="Upgrade" />
												</div>
											</div>
										</form>
									</div>
								</div>
							</div>
						</div>
					</div>
				</div>
			</div>
		</div>
	</div>
	<script src="/common.js"></script>
	<script src="https://code.jquery.com/jquery-3.6.0.min.js" integrity="sha256-/xUj+3OJU5yExlq6GSYGSHk7tPXikynS7ogEvDej/m4=" crossorigin="anonymous"></script>
	<script>
		function regenerateActivationKey()
		{
			if(confirm("Are you absolutely sure you need to regenerate your activation key? E.g., you shared the activation key and now want to take it back."))
			{
				$.post("/api/regenerate",{
					account_id: localStorage.getItem("account_id")
				}).done(function(data)
				{
					if (localStorage.getItem("activation_key") == data)
					{
						alert("Sorry, you've regenerated your key too often. Try again later.");
					}
					else
					{
						update_activation_key(data);
						updateActivationKey();
					}
				});
			}
		}

		function submit_upgrade()
		{
			$.post("/api/upgrade",{
				account_id: localStorage.getItem("account_id"),
				upgrade_key: document.querySelector("#upgrade-key").value.substr(14)
			}).done(function(data)
			{
				if(data.substr(0, 8) == "upgrade ")
				{
					localStorage.setItem("account_privilege", data.substr(8));
					updatePrivilege();
				}
				else if(data == "mismatch")
				{
					alert("Upgrade key does not apply to your edition");
				}
				else if(data == "invalid_or_used")
				{
					alert("Invalid upgrade key or already used");
				}
				else
				{
					alert("Upgrade failed");
				}
			});
		}

		function kickOffDiscordAction(action)
		{
			$.post("/api/request_link_token", {
				account_id: localStorage.getItem("account_id")
			}).done(function(link_token)
			{
				location.href="https://backend-services.stand.sh/discord/"+action+"#"+link_token;
			}).fail(function()
			{
				alert("Failed to initiate Discord action. Try logging out of your Stand account and back in again.");
			});
		}

		function updateActivationKey()
		{
			document.querySelector("#activation-key").value="Stand-Activate-"+localStorage.getItem("activation_key");
			document.querySelector("#discord-join-link").onclick = function()
			{
				kickOffDiscordAction("join");
			};
			document.querySelector("#discord-update-link").onclick = function()
			{
				kickOffDiscordAction("update");
			};
			document.querySelector("#web-interface-link").href="/web#"+localStorage.getItem("activation_key");
		}

		function updatePrivilege()
		{
			document.querySelectorAll(".suspended-no, .suspended-yes, .privilege-1, .privilege-2, .privilege-3, .no-coins, .have-coins").forEach(elm=>{
				elm.classList.add("d-none");
			});
			if(localStorage.getItem("account_suspended_for"))
			{
				document.getElementById("suspended-for").textContent = localStorage.getItem("account_suspended_for");
				document.querySelectorAll(".suspended-yes").forEach(elm=>{
					elm.classList.remove("d-none");
				});
				return false;
			}
			document.querySelectorAll(".suspended-no").forEach(elm=>{
				elm.classList.remove("d-none");
			});
			document.querySelectorAll(".privilege-"+localStorage.getItem("account_privilege")).forEach(elm=>{
				elm.classList.remove("d-none");
			});
			document.querySelectorAll(localStorage.getItem("account_coins")==0?".no-coins":".have-coins").forEach(elm=>{
				elm.classList.remove("d-none");
			});
			return true;
		}

		function updateAccountData()
		{
			document.getElementById("quiz-finisher").classList.add("d-none");

			updateActivationKey();
			document.querySelector("#activation-key").onfocus=function()
			{
				this.select();
			};
			document.querySelector("#activation-key").onclick=function()
			{
				let val=this.value;
				if(val=="Copied to clipboard!")
				{
					return;
				}
				this.select();
				document.execCommand("copy");
				this.value="Copied to clipboard!";
				setTimeout(function()
				{
					document.querySelector("#activation-key").value=val;
				},1000);
			};

			if(updatePrivilege())
			{
				document.getElementById("balance").textContent=format_coins(localStorage.getItem("account_coins"));

				if (localStorage.getItem("account_created_quiz_success") === "false")
				{
					document.getElementById("quiz-finisher").classList.remove("d-none");
				}
			}
		}

		function censor_id(id)
		{
			return id.substr(0,6)+'*'.repeat(id.length-6);
		}

		function logout()
		{
			localStorage.removeItem("account_id");
			forget_account_data();
			localStorage.removeItem("account_id_downloaded");

			document.querySelector("#logged-in").classList.add("d-none");
			document.querySelector("#logged-out").classList.remove("d-none");
		}

		if(typeof localStorage.getItem("account_id")=="string")
		{
			document.querySelector("#logged-in").classList.remove("d-none");
			document.querySelector("#logged-out").classList.add("d-none");
			updateAccountData();
			document.querySelector("#account-id").textContent=censor_id(localStorage.getItem("account_id"));
			document.querySelector("#reveal-account-id").onclick=function()
			{
				if(confirm("Are you sure you want to see your full account id? You MUST NEVER SHARE this. NO MATTER WHO ASKS."))
				{
					document.querySelector("#account-id").textContent=localStorage.getItem("account_id");
					this.classList.add("d-none");

					// No cheating
					document.getElementById("quiz-finisher").classList.add("d-none");
				}
			};
			if(time() - localStorage.getItem("last_account_data_update") > (60 * 60)
				&& location.hostname != "localhost"
				)
			{
				$.post("/api/basic_account_info", {
					account_id: localStorage.getItem("account_id")
				}).done(function(data)
				{
					if (data != "bad")
					{
						store_account_data(data);
						updateAccountData();
					}
					else
					{
						logout();
					}
				}).fail(function()
				{
					logout();
				});
			}
		}
	</script>
</body>
</html>
