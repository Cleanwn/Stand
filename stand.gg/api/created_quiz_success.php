<?Php
if(empty($_POST["account_id"])||strlen($_POST["account_id"])!=31)
{
	http_response_code(400);
	exit;
}
require "../src/include.php";
$res = $db->query("UPDATE `accounts` SET `created_quiz_success`=1 WHERE `id`=?", "s", $_POST["account_id"]);
