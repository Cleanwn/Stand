// Challenger

not ssl
or not cf.edge.server_port in {443}
or http.request.uri.path contains "//"
or (
    http.request.version in {"HTTP/1.0" "HTTP/1.1"}
    and not (
        http.user_agent eq "Mozilla/5.0 (compatible; Soup Library; +https://soup.do)"
        or http.user_agent eq "Mozilla/5.0 (compatible; calamity-inc/Soup)"
        or http.user_agent eq ""
        or http.user_agent contains "MSIE"
        or http.user_agent contains "WindowsPowerShell"
        or http.user_agent contains "NintendoBrowser/"
        or cf.client.bot
        or http.user_agent eq "StandParser"
        or http.user_agent eq "GhostmotherBot/1.0"
    )
)

// Blocker

not http.request.version in { "HTTP/1.0" "HTTP/1.1" "HTTP/1.2" "HTTP/2" "HTTP/3" "SPDY/3.1" }
or not http.request.method in {"GET" "POST" "HEAD" "OPTIONS"}
or http.user_agent in {
    "undefined"
    "CheckHost (https://check-host.net/)"
}
or http.user_agent contains "Uptime"
or (
    not http.request.method in {"GET" "HEAD"}
    and (
        not http.request.uri.path in {
            "/api/basic_account_info"
            "/api/brain_replacement"
            "/api/buy_vpn_days"
            "/api/created_quiz_success"
            "/api/event"
            "/api/heartbeat"
            "/api/logout"
            "/api/migrate"
            "/api/my_packages"
            "/api/package_update"
            "/api/redeem"
            "/api/redeem_topup"
            "/api/regenerate"
            "/api/request_link_token"
            "/api/secured_content"
            "/api/session_code_create"
            "/api/simple_update"
            "/api/topup"
            "/api/upgrade"
            "/keymaster/account-lookup"
            "/keymaster/activation-lookup"
            "/keymaster/discord-lookup"
            "/keymaster/license-lookup"
        }
        or http.host ne "stand.sh"
    )
)
or (
    http.request.uri.path in {
        "/api/event"
        "/api/heartbeat"
        "/api/logout"
        "/api/secured_content"
        "/api/session_code_create"
    }
    and (
        http.request.method ne "POST"
        or http.host ne "stand.sh"
        or (
            not (
                (http.request.version eq "HTTP/1.0" or http.request.version eq "HTTP/1.1")
                and (http.user_agent eq "Mozilla/5.0 (compatible; Soup Library; +https://soup.do)" or http.user_agent eq "Mozilla/5.0 (compatible; calamity-inc/Soup)")
            )
        )
    )
)
or (
    http.request.uri.path in {
        "/api/basic_account_info"
        "/api/regenerate"
        "/api/request_link_token"
        "/api/simple_update"
    }
    and http.request.method ne "POST"
)
or (
    not http.request.method in {"GET" "HEAD"}
    and not http.cookie contains "a=1"
    and not (
        (
            (http.request.version eq "HTTP/1.0" or http.request.version eq "HTTP/1.1")
            and (http.user_agent eq "Mozilla/5.0 (compatible; Soup Library; +https://soup.do)" or http.user_agent eq "Mozilla/5.0 (compatible; calamity-inc/Soup)")
        )
    )
)

// Bypasser

http.request.version eq "HTTP/1.1" and (http.user_agent eq "Mozilla/5.0 (compatible; Soup Library; +https://soup.do)" or http.user_agent eq "Mozilla/5.0 (compatible; calamity-inc/Soup)")

/* Cache Purge for Sellers Update:

https://stand.sh/help/basic-edition
https://stand.sh/help/regular-edition
https://stand.sh/help/ultimate-edition
https://stand.sh/help/basic-to-regular-edition
https://stand.sh/help/regular-to-ultimate-edition
https://stand.sh/help/basic-to-ultimate-edition

/* Cache Purge for Stand Update:

https://stand.sh/stand-versions.txt
https://stand.sh/versions.txt
https://stand.sh/help/supported-versions
https://stand.sh/help/changelog
https://stand.sh/help/changelog-launchpad

/* Cache Purge for Features Update:

https://stand.sh/help/command-list
https://stand.sh/components/command-list.txt
https://stand.sh/web

/* Cache Purge for Repository Update:

https://stand.sh/api/tuna.json

/* Cache Purge for Editions Update:

https://stand.sh/
https://stand.sh/account/register

/* Cache Purge for Launchpad Update:

https://stand.sh/versions.txt
https://stand.sh/help/changelog
https://stand.sh/help/changelog-launchpad
