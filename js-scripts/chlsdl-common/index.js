// ==UserScript==
// @name        chlsdl-common
// @namespace   Violentmonkey Scripts
// @version     1.0
// @author      usadev1984
// @description 3/22/2026, 11:25:00 AM
// ==/UserScript==

function chlsdl_send_to_chlsdl(download_url, json_data) {
    /*
     * need this to try to reduce the amount of stuff sent in the request since
     * we don't care about any of it yet
     */
    const headers = {
        "User-Agent": "",
        "Accept": "",
        "Accept-Language": "",
        "Accept-Encoding": "",
        "Content-Type": "",
        "Content-Length": "",
        "Sec-GPC": "",
        "Connection": "",
        "Sec-Fetch-Dest": "",
        "Sec-Fetch-Mode": "",
        "Sec-Fetch-Site": "",
    };

    const final_data = download_url + "\n" + JSON.stringify(json_data);

    GM_xmlhttpRequest({
        url: "http://localhost:53162",
        method: "POST",
        anonymouse: true,
        /**
         * setting a timeout appears to be useless since `onerror` gets called
         * instead when we can't connect to the main program because it
         * didn't have a socket open.
         * unrelated: https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/readyState
         */
        // timeout: 2000,
        // ontimeout: (function (x) {
        //     console.log(x)
        //     console.error("post request timed out");
        // }),
        onerror: (function (x) {
            console.error("failed to post data. is the main program running?");
            console.log(x);
        }),
        headers: headers,
        data: final_data,
    });
}
