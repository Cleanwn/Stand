<!DOCTYPE html>
<html>
<head>
	<title>Changelog | Stand</title>
	<meta name="viewport" content="width=device-width, initial-scale=1">
	<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/uikit@3.6.11/dist/css/uikit.min.css" crossorigin="anonymous" />
	<style>
		html, body {
			background-color: #101110;
			color: #fefffe;
		}

		h2 {
			color: #dcdcdc;
		}
	</style>
</head>
<body>
	<div class="uk-container uk-container-xlarge uk-margin-top uk-margin-bottom">
		<?php
		function printHeader($name, $date)
		{
			echo '<h2>'.$name.' <span class="uk-text-meta">'.$date.'</span></h2>';
		}
		require "../components/changelog.php";
		?>
	</div>
</body>
</html>
