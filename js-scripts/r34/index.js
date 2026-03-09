// ==UserScript==
// @name        chlsdl-r34
// @namespace   Violentmonkey Scripts
// @match       https://rule34.xxx/index.php*
// @grant       clipboardWrite
// @grant       GM_setClipboard
// @version     1.0
// @author      usadev1984
// @description 3/9/2026, 10:43:14 PM
// ==/UserScript==
(function() {
    'use strict';

    document.onkeyup = function(e) {
        if (e.which != 192) // `
            return;

        scrape();
    }

    function scrape() {
        let url = document.querySelector('video#gelcomVideoPlayer > source');
        if (url == null) {
            url = document.querySelector('meta[property="og:image"]');
            if (url == null) {
                alert('url not found"' +url +'"')
                return
            }
            url = url.content;
        } else
            url = url.src;

        url = url.replace(/\?[0-9]+$/, '');

        let name = url.match(/(?:.+)\/(.*)/)
        name = name[name.length - 1];

        let source = window.location.href.match(/.+(?<=[\?&])id=[0-9]+/)[0];

        var postlist = document.querySelectorAll('article[id^="post_"]');
        GM_setClipboard(url + "\n" + JSON.stringify({
            "url": url,
            "name": name,
            "copyright": get_post_tags('copyright'),
            "character": get_post_tags('character'),
            "artist": get_post_tags('artist'),
            "general": get_post_tags('general'),
            "meta": get_post_tags('metadata'),
            "source": source,
            "information": get_post_info(),
        }), "text/plain");
    }

    function get_post_tags(which_tags) {
        let elems = document.querySelectorAll(
            '#tag-sidebar > .tag-type-' + which_tags + '.tag > a[href*="tags="]');
        if (elems.length == 0)
            console.warn("couldn't find tags of type:", which_tags);
        return Array.from(elems).map(x => x.innerText);
    }

    function get_post_info() {
        let infolist = document.querySelectorAll("#stats > ul > li")
        if (infolist.length == 0) {
            console.warn('post information not found: "' + infolist +'"');
            return [];
        }

        let r = [];
        r.push(infolist[0].innerText.replace('Id: ', ''));
        r.push(infolist[1].innerText.match(
            /.+[0-9]{2}:[0-9]{2}/)[0].replace("Posted: ", '')); // date
        r.push(Array.from(infolist[3].querySelectorAll('a')).map(x => x.href)); // source(s)
        r.push(infolist[4].innerText.replace('Rating: ', ''));

        return r;
    }
})();
