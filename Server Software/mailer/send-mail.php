<?php
require "vendor/autoload.php";

$email = Email\Email::basic(
    new Email\Address("Your Brain Replacement <your-brain-replacement@stand.sh>"),
    new Email\Address($_POST["r"]),
    /* Subject: */ "Your Stand Account ID",
    new Email\ContentTextPlain("Your Stand Account ID: ".$_POST["i"]."\r\n\r\nDon't share this with anyone!\r\n\r\nLogin: https://stand.sh/account/login")
)->sign(new Email\DkimKey("a", "file://".__DIR__."/dkim-private.pem"));

$con = $email->getRecipient()->createConnection(
    Email\Client::DEFAULT_CONNECT_TIMEOUT,
    Email\Client::DEFAULT_READ_TIMEOUT,
    Email\Connection::LOGFUNC_ECHO
);

if($con instanceof Email\Connection)
{
    $con->identify(function() use ($con, $email)
    {
        $con->sendEmail($email, function() use ($con)
        {
            $con->close();
        }, function(Email\Fail $fail) use ($con)
        {
            $con->close();
        });
    });
}

Asyncore\Asyncore::loop();
