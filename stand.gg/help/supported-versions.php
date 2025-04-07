<!DOCTYPE html>
<html>
<head>
	<title>Supported Versions | Stand</title>
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
					<li class="nav-item"><a class="nav-link" href="/account/">Account</a></li>
					<li class="nav-item active"><a class="nav-link" href="/help/">Help</a></li>
				</ul>
			</div>
		</nav>
		<div class="content-wrapper">
			<div class="container-xl">
				<div class="content">
					<h1>Supported Versions</h1>
					<p>The following versions of Stand are currently supported:</p>
					<ul>
						<!-- <li><a href="/Stand WHATEVER.dll" download>Stand WHATEVER</a></li> -->
						<?php
							$versions = file("../stand-versions.txt", FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
							echo '<li><b><a href="/Stand ' . $versions[0] . '.dll" download>Stand ' . $versions[0] . '</a> (latest)</b></li>';
							if (count($versions) == 1)
							{
								echo '<li>No alternatives are available at the moment</li>';
							}
							else
							{
								for ($i = 1; $i < count($versions); $i++)
								{
									echo '<li><a href="/Stand ' . $versions[$i] . '.dll" download>Stand ' . $versions[$i] . '</a></li>';
								}
							}
						?>
					</ul>
					<h3>How to use an alternative version</h3>
					<p>Just download the file here and use the Launchpad's Advanced Mode to inject it.</p>
					<p><img src="/help/assets/launchpad-advanced-add.png" /></p>
					<p>Note that you have to set it so that it only injects a single Stand version.</p>
					<p><img src="/help/assets/launchpad-advanced-checkboxes.png" /></p>
				</div>
			</div>
		</div>
	</div>
</body>
</html>
