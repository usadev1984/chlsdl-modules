// ==UserScript==
// @name        chlsdl-r34
// @namespace   Violentmonkey Scripts
// @match       https://rule34.xxx/index.php*
// @icon        data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAy0lEQVQ4T6WT6xXDIAiFJSvpTHUmO5OuFAum14OG2JzGP0YeH1ckVGulEMPu/lg55Y38y1fJ5UNHEFH7Zt8JK3Ha3wDaKBmsaggCBUVOgPIuQyULgCKzrykAAJJnmfpsAlY9WMEkrzdRX0NXke+rJb34CZiTNbwrgMyrHgAiKk0AHAjEM1mzYM7B/IwAWYM0v5jZA1QuZZwP770TgIDhO+aAA1NOLlJ0qR674ym+Y+sAx8V0crvGDdtzBfI7U6Bdqonsvn8VrGycu30ARcXJLW7rOcUAAAAASUVORK5CYII=
// @grant       clipboardWrite
// @grant       GM_setClipboard
// @grant       GM_xmlhttpRequest
// @require     https://raw.githubusercontent.com/usadev1984/chlsdl-modules/refs/heads/latest/js-scripts/chlsdl-common/index.js
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

        chlsdl_send_to_chlsdl(url, {
            "url": url,
            "name": name,
            "copyright": get_post_tags('copyright'),
            "character": get_post_tags('character'),
            "artist": get_post_tags('artist'),
            "general": get_post_tags('general'),
            "meta": get_post_tags('metadata'),
            "source": source,
            "information": get_post_info(),
        });
    }

    function get_post_tags(which_tags) {
        let elems = document.querySelectorAll(
            '#tag-sidebar > .tag-type-' + which_tags + '.tag > a[href*="tags="]');
        if (elems.length == 0)
            console.warn("couldn't find tags of type:", which_tags);
        return Array.from(elems).map(x => x.innerText);
    }

    function get_post_info() {
        var xpath = '//*[@id="stats"]/ul/li';
        var stats = document.evaluate(
            `concat(
                ${xpath}[starts-with(text(), "Id: ")], '\n',
                substring(normalize-space(${xpath}[starts-with(
                            normalize-space(text()), "Posted: ")]), 1, 27), '\n',
                ${xpath}[starts-with(text(), "Source: ")], '\n',
                ${xpath}[starts-with(text(), "Rating: ")]
            )`,
            document, null, XPathResult.STRING_TYPE, null);

        console.log(stats.stringValue);

        var r = {};
        for (const i of stats.stringValue.split('\n')) {
            if (i == '')
              continue;
            var [key, value] = i.split(': ');
            /* ... */
            key = key.toLowerCase();
            if (key == "source")
                value = value.split(' ')
            else if (key == "posted")
                key = "date";
            r[key] = value;
        }

        console.log(r)
        return r;
    }
})();
