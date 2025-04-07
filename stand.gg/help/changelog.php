<!DOCTYPE html>
<html>
<head>
	<title>Changelog | Stand</title>
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<script>
		if(location.search=="?Launchpad")
		{
			location.href="changelog-launchpad";
		}
	</script>
	<link href="/halfmoon-variables.min.css" crossorigin="anonymous" rel="stylesheet" />
	<link href="/halfmoon-config.css" rel="stylesheet" />
</head>
<body class="dark-mode">
	<div class="page-wrapper with-navbar">
		<nav class="navbar">
			<div class="container-xl">
				<a class="navbar-brand" href="/">Stand</a>
				<ul class="navbar-nav">
					<li class="nav-item"><a class="nav-link" href="/account/">Account</a></li>
					<li class="nav-item active"><a class="nav-link" href="/help/">Help</a></li>
				</ul>
			</div>
		</nav>
		<div class="content-wrapper">
			<div class="container-xl">
				<div class="content">
					<h1>Stand Changelog</h1>
					<?php
					function printHeader($name, $date)
					{
						echo '<h3>'.$name.' <span class="font-size-18 text-muted">'.$date.'</span></h3>';
					}
					require "../components/changelog.php";
					?>
				</div>
			</div>
		</div>
	</div>
</body>
</html>
