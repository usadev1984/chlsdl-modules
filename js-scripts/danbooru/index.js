// ==UserScript==
// @name        chlsdl-danbooru
// @namespace   Violentmonkey Scripts
// @match       https://danbooru.donmai.us/posts/*
// @grant       clipboardWrite
// @grant       GM_setClipboard
// @version     1.0
// @author      -
// @description 11/15/2025, 10:43:14 AM
// ==/UserScript==
(function() {
    'use strict';

    document.onkeyup = function(e) {
        if (e.which != 192) { // `
            return;
        }

        scrape();
    }

    function scrape() {
        let url = document.querySelector('#content > #image-resize-notice > a[href]')
        if (url == null) {
            url = document.querySelector('#image[src]')
            if (url == null) {
                alert('url not found"' +url +'"')
                return
            }
            console.log(url)
            url = url.src
        } else {
            console.log(url)
            url = url.href
        }

        let name = url.match(/(?:.+)\/(.*)/)
        name = name[name.lastIndex]

        let source = window.location.href
        console.log(url + '\n' + source);
        source = source.replace(/\?.*/, '')

        let post_info = get_post_info();

        var postlist = document.querySelectorAll('article[id^="post_"]');
        var parent_id = null
        var children = null
        var siblings = null
        if (!(postlist == null)) {
            var posts = Array.from(postlist, x => x.id.replace('post_', ''));

            var paren = (function() {
                var parent_or_child = Array.from(postlist, x =>
                    x.className.includes('post-status-has-children'));

                var idx = parent_or_child.indexOf(true)
                var post = posts[idx]
                posts.splice(idx, 1);
                return post;
            })();

            if (paren == post_info[0]) {
                console.warn('parent')
                children = posts
            } else {
                console.warn('child')
                parent_id = paren;
                siblings = posts.filter((x) => x != post_info[0]);
            }
        }

        console.log("parent_id", parent_id, "\n", "children", children, "\n", "siblings", siblings)

        GM_setClipboard(url + "\n" + JSON.stringify({
            "url": url,
            "name": name,
            "source": source,
            "information": post_info,
            "parent_id": parent_id,
            "children": children,
            "siblings": siblings,
        }), "text/plain");
    }

    function get_post_info() {
        let infolist = document.querySelectorAll("#post-information > ul > li")
        if (infolist.length == 0) {
            alert('post information not found: "' + infolist +'"')
            return []
        }

        infolist = Array.from(infolist).filter(
            (x) => ['post-info-id', 'post-info-date', 'post-info-source', 'post-info-rating'].includes(x.id))
        infolist[0] = infolist[0].innerText.replace(/ID: /, '') // post id
        infolist[1] = Date.parse(infolist[1].querySelector('time').dateTime) / 1000 // date
        infolist[2] = infolist[2].querySelector('a[href]').href // source
        infolist[3] = infolist[3].innerText.replace(/Rating: /, '') // rating

        return infolist
    }
})();
