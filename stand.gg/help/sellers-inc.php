<?php
if(empty($product))
{
	die("No.");
}

header("Cache-Control: public, max-age=7200");

// 100 XXX in EUR (Last updated: 2024-09-06)
$to_euro = [
	"$" => (90.01 / 100), // USD
	"£" => (118.60 / 100), // GBP
	"R$" => (16.16 / 100), // BRL
	"元" => (12.71 / 100), // RMB 欧元
	"₩" => (0.068 / 100), // KRW
	"₽" => (1.00 / 100), // RUB
];

function parsePrice(string $price): array
{
	$currency_symbol = mb_substr($price, 0, 1);
	if($currency_symbol == "R")
	{
		return ["R$", floatval(mb_substr($price, 2))];
	}
	return [$currency_symbol, floatval(mb_substr($price, 1))];
}

function stringifyPrice(array $price)
{
	$decimals = 2;
	if($price[0] == "元" || $price[0] == "₽")
	{
		$decimals = 0;
	}
	$str = number_format($price[1], $decimals, ",", ".");
	if($price[0] == "€" || $price[0] == "元" || $price[0] == "₽")
	{
		$str .= $price[0];
	}
	else
	{
		$str = $price[0].$str;
	}
	return $str;
}

require "../src/dbinclude.php";

function rowsToArray($rows)
{
	$arr = [];
	foreach($rows as $row)
	{
		$arr[$row["note"]] = $row["count"];
	}
	return $arr;
}

$stock = [
	"basic" => rowsToArray($db->query("SELECT note, COUNT(*) as count FROM license_keys WHERE `privilege`=1 AND `used_by`='' GROUP BY note")),
	"regular" => rowsToArray($db->query("SELECT note, COUNT(*) as count FROM license_keys WHERE `privilege`=2 AND `used_by`='' GROUP BY note")),
	"ultimate" => rowsToArray($db->query("SELECT note, COUNT(*) as count FROM license_keys WHERE `privilege`=3 AND `used_by`='' GROUP BY note")),
	"basic_to_regular" => rowsToArray($db->query("SELECT note, COUNT(*) as count FROM upgrades WHERE `privilege_from`=1 AND `privilege_to`=2 AND `used_by`='' GROUP BY note")),
	"regular_to_ultimate" => rowsToArray($db->query("SELECT note, COUNT(*) as count FROM upgrades WHERE `privilege_from`=2 AND `privilege_to`=3 AND `used_by`='' GROUP BY note")),
	"basic_to_ultimate" => rowsToArray($db->query("SELECT note, COUNT(*) as count FROM upgrades WHERE `privilege_from`=1 AND `privilege_to`=3 AND `used_by`='' GROUP BY note")),
];

$hard_price_bottom = [
	"basic" => 12.49,
	"regular" => 24.99,
	"ultimate" => 49.99,
	"basic_to_regular" => 12.49,
	"regular_to_ultimate" => 24.99,
	"basic_to_ultimate" => 37.49,
];

$json = json_decode(file_get_contents("sellers.json"), true);
if($json)
{
	$paymets = [];
	foreach($json as $seller_name => $seller_products)
	{
		if(array_key_exists($product, $seller_products))
		{
			$seller_stock = 0;
			if(array_key_exists($product, $stock))
			{
				$seller_dbnames = [$seller_name];
				if(array_key_exists("aliases", $seller_products))
				{
					$seller_dbnames = array_merge($seller_dbnames, $seller_products["aliases"]);
				}
				if(array_key_exists("stock", $seller_products[$product]))
				{
					$seller_stock += $seller_products[$product]["stock"];
				}
				foreach($seller_dbnames as $dbname)
				{
					if(array_key_exists($dbname, $stock[$product]))
					{
						$seller_stock += $stock[$product][$dbname];
					}
				}
				if($seller_stock <= 0)
				{
					continue;
				}
			}
			foreach($seller_products[$product] as $payname => $paydata)
			{
				if($payname == "stock" || $payname == "_stock")
				{
					continue;
				}
				/*if (!$paydata[0])
				{
					die("invalid paydata on $seller_name for $product");
				}*/
				$price = parsePrice($paydata[0]);
				$price_converted = $price[1];
				$price_display = stringifyPrice($price);
				if($price[0] != "€")
				{
					$price_converted = ($to_euro[$price[0]] * $price[1]);
					if($price[0] == "元" || $price[0] == "₽")
					{
						$price_display .= " (~".stringifyPrice(["€", $price_converted]).")";
					}
					else
					{
						$price_display = "~".stringifyPrice(["€", $price_converted])." (".$price_display.")";
					}
				}
				if (array_key_exists($product, $hard_price_bottom)
					&& $price_converted < $hard_price_bottom[$product]
					)
				{
					$price_converted = $hard_price_bottom[$product];
				}
				$weight = $price_converted;
				if ($seller_stock < 5)
				{
					$weight += 1;
				}
				else
				{
					$weight -= ($seller_stock - 1) * 0.01;
				}
				$paymets[$payname][] = [
					$weight,
					"<b>".$price_display."</b> — ".$seller_name,
					$paydata[1],
					$seller_stock,
				];
			}
		}
	}

	function printPaymet(string $display_name, string $code_name): bool
	{
		global $paymets;

		if(array_key_exists($code_name, $paymets))
		{
			?>
			<div class="col-xl-3 col-md-6">
				<div class="card p-20 m-10">
					<h2 class="card-title"><?=$display_name; ?></h2>
					<ul style="min-width:290px;max-width:calc(100vw - 50px)">
						<?php
						usort($paymets[$code_name], function($a, $b)
						{
							return $a[0] <=> $b[0];
						});
						foreach($paymets[$code_name] as $seller)
						{
							echo '<li><a data-weight="'.$seller[0].'" data-stock="'.$seller[3].'" href="'.$seller[2].'">'.$seller[1].'</a></li>';
						}
						?>
					</ul>
				</div>
			</div>
			<?php
			return true;
		}
		return false;
	}

	echo '<div class="grid" data-masonry=\'{"itemSelector": ".col-xl-3"}\'>';

	printPaymet("Credit Card", "cc");
	printPaymet("PayPal", "pp");
	printPaymet("支付宝 (支持花呗、信用卡)", "alipay");
	printPaymet("Bitcoin (BTC)", "btc");

	printPaymet("Google Pay", "google");
	printPaymet("Apple Pay", "apple");
	printPaymet("微信支付 (支持信用卡)", "wechat");
	printPaymet("QQ", "qq");

	printPaymet("Cashapp", "cashapp");
	printPaymet("Venmo", "venmo");
	printPaymet("Skrill", "skrill");

	printPaymet("Litecoin (LTC)", "ltc");
	printPaymet("Ethereum (ETH)", "eth");
	printPaymet("Bitcoin Cash (BCH)", "bch");
	printPaymet("DAI, USDC, DOGE", "crypto2");
	// Have "xmr" for Monero, but only 1 reseller with it.

	printPaymet("PaySafeCard", "psc");
	printPaymet("Amazon Gift Card (USA)", "amazongift");
	printPaymet("Perfect Money", "perfect");
	printPaymet("Amazon Pay", "amazon");

	printPaymet("Europe Bank Transfer (SEPA, iDEAL, EPS, Bancontact, Przelewy24)", "europe"); // Ares no longer accepts "Sofort"
	printPaymet("LATAM Bank Transfer (Pix, PagSeguro)", "latam");
	printPaymet("Wire Transfer via Wise", "wise");
	printPaymet("BNPL (Klarna and/or Affirm)", "klarna"); // Also have "affirm" specifically, but who cares?
	printPaymet("Multibanco", "multibanco");
	printPaymet("ADA", "ada");
	printPaymet("Mercado Pago", "mercado"); // Like PayPal but for LATAM
	printPaymet("Boleto", "boleto");
	printPaymet("Bank Transfer via GoCardless", "gocardless");
	printPaymet("BNB", "bnb");
	printPaymet("UPI, PayTM", "india");
	printPaymet("Korea Bank Transfer, Culture Land", "korea");
	printPaymet("Tinkoff, Sberbank, SBP, YooMoney", "russia");
	printPaymet("PicPay", "picpay"); // Another LATAM PayPal clone; smaller & Brazil-specific tho

	echo '</div><script src="https://unpkg.com/masonry-layout@4/dist/masonry.pkgd.min.js"></script>';
}
else
{
	echo "<p>Oopise woopsie, looks like we did a fucky-wucky. Please let us know if see this message, lol.</p>";
}
