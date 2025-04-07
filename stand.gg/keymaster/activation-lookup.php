<?php
require __DIR__."/src/include.php";
?>
<title>Activation Key Lookup | Stand KeyMaster</title>
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
		<h1>Activation Key Lookup</h1>
		<form method="POST">
			<input type="hidden" name="auth" />
			<input type="text" name="key" placeholder="Activation key" minlength="1" required />
			<input type="submit" class="slv-button" value="Lookup" />
		</form>
		<?php
		if (isDynamicRequest())
		{
			echo "<hr>";
			try
			{
				checkAccess();
				$key = simplifyKey($_POST["key"]);
				if ($acctid = getAccountIdByActivationKey($key))
				{
					echo "<p><code>Stand-Activate-$key</code> belongs to ".acctidToHtml(collapseAccountId($acctid)).".</p>";
				}
				else
				{
					echo "<p>Activation key is not valid (anymore).</p>";
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
