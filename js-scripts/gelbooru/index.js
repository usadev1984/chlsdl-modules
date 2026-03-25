// ==UserScript==
// @name        chlsdl-gelbooru
// @namespace   Violentmonkey Scripts
// @match       https://gelbooru.com/index.php*
// @icon        data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAACDElEQVQ4T4WTT2gTQRSHv5l0tVQ9lEKpqCCCCkpBxYOVVtBQsUg9FEGKBEOhIiooHnornutFUIgKSqJELyp4sRD/oIcWFAMeJAWValWkULGHItYkuzu+Sdw2azf6YJaZnXnfvPd+bxRn3EMovxdooCxfT0aUOfIzFtrwQT9UnC3dwpDglywbYUubqozmJigLbHrOUJiGz98MiAvLQ5Cs4kQprRyS5/ZqTu/RrG9RS+4XV159Mlwb87md9ylakD2myKhE1ksf71DJzg1LHaMy2XXR5eWkIBv+AIwxaZkmaw8/fWt49t4wXzaVdHrbFa0rqxf8E1B0oe+6y+hruSEISKZNK2CoO8b5Hk3XJZexd3UiOHXPI5WTBMUhZLYI8xDfppiaNUzOyFr/lcLsT1g9XKYkUYgq0XJamZdVBK+aLWJQg/GPhs4L4i0HWlfB9rXRRc1/MXz/UaNCABidMBy8LADx69upuD8QXBPO5sBVl9wbCdE2Vm0EL6YMHSP/B3SnXJ4UIgALNShC+zrF4G6NEd7WNYr4psV06gJsoCfvelx5JCpIS1OSIbD+uOZOYvERRAEycvSYBVgFjmRcHuQlRKuEQI7u02TrA26qomtuODEGams+/sHwXDrxq2jetVnRv8OKXrVQBJBWhRlzeGMLPY6utMaC+ZLJnLzQRqm2HYHtT3k8npBNR96m0rnfHVnyEvoH+mwAAAAASUVORK5CYII=
// @grant       clipboardWrite
// @grant       GM_setClipboard
// @grant       GM_xmlhttpRequest
// @require     https://raw.githubusercontent.com/usadev1984/chlsdl-modules/refs/heads/latest/js-scripts/chlsdl-common/index.js
// @version     1.0
// @author      usadev1984
// @description 3/22/2026, 5:12:06 PM
// ==/UserScript==
(function() {
    'use strict';

    document.onkeyup = function(e) {
        if (e.which != 192) // `
            return;

        scrape();
    }

    function scrape() {
        let url = document.querySelector('video#gelcomVideoPlayer > source[type="video/mp4"]');
        if (url == null) {
            url = document.querySelector('meta[property="og:image"]');
            if (url == null) {
                alert('url not found"' +url +'"')
                return
            }
            url = url.content;
        } else
            url = url.src;

        /* name */
        const name = (() => {
            const pathname = new URL(url).pathname;
            return pathname.slice(pathname.lastIndexOf('/') + 1);
        })();

        console.log(name);

        /* source */
        var source = new URL(window.location.href);
        console.log(source.toString());

        const allowed_params = ["page", "s", "id"];
        for (const i of source.searchParams.keys())
          if (!allowed_params.includes(i))
              source.searchParams.delete(i);

        source = source.toString();
        console.log(source);

        chlsdl_send_to_chlsdl(url, {
            "url": url,
            "src": source,
            "name": name,
            "copyright": get_taglist('copyright'),
            "character": get_taglist('character'),
            "artist": get_taglist('artist'),
            "general": get_taglist('general'),
            "metadata": get_taglist('metadata'),
        });
    }

    function get_taglist(list_class) {
        const elems = document.querySelectorAll(
            '#tag-list > .tag-type-' + list_class + ' > a[href*="tags="]');
        if (elems.length == 0)
            console.warn("couldn't find tags of type:", list_class);
        return Array.from(elems).map(x => x.innerText);
    }
})();
