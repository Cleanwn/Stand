<?php
require __DIR__."/src/include.php";
?>
<title>Discord User Lookup | Stand KeyMaster</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="stylesheet" href="./slv/base.css">
<body>
	<div class="slv-sidebar">
		<h3 class="slv-sidebar-brand">KeyMaster</h3>
		<a href="./account-lookup" class="slv-item">Account Lookup</a>
		<a href="./activation-lookup" class="slv-item">Activation Key Lookup</a>
		<a href="./discord-lookup" class="slv-item">Discord User Lookup</a>
		<a href="./license-lookup" class="slv-item">License Key Lookup</a>
	</div>
	<div>
		<h1>Discord User Lookup</h1>
		<form method="POST">
			<input type="hidden" name="auth" />
			<input type="text" name="discid" placeholder="Discord user id" minlength="1" required />
			<input type="submit" class="slv-button" value="Lookup" />
		</form>
		<?php
		if (isDynamicRequest())
		{
			echo "<hr>";
			try
			{
				checkAccess();
				if ($arr = getAccountsByDiscordUser($_POST["discid"]))
				{
					printDiscordUserInfo($arr);
				}
				else
				{
					echo "<p>Found nothing.</p>";
				}
			}
			catch (Exception $e)
			{
				echo "<p>".$e->getMessage()."</p>";
			}
		}
		?>
	</div>
</body>
<script src="./script.js"></script>
<script src="./timestamp.js"></script>
<script>
	if (location.hash.replace("#", ""))
	{
		document.querySelector("[name='discid']").value = location.hash.replace("#", "");
		location.hash = "";
		document.querySelector("[type='submit']").click();
	}
</script>
