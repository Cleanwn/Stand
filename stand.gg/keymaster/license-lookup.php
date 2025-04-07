<?php
require __DIR__."/src/include.php";
?>
<title>License Key Lookup | Stand KeyMaster</title>
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
		<h1>License Key Lookup</h1>
		<form method="POST">
			<input type="hidden" name="auth" />
			<input type="text" name="key" placeholder="License key" minlength="1" required />
			<input type="submit" class="slv-button" value="Lookup" />
		</form>
		<?php
		if (isDynamicRequest())
		{
			echo "<hr>";
			try
			{
				checkAccess();
				if ($key = lookupLicenseKey($_POST["key"]))
				{
					printKeyInfo($key);
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
