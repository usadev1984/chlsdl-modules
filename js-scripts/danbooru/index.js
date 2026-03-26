// ==UserScript==
// @name        chlsdl-danbooru
// @namespace   Violentmonkey Scripts
// @match       https://danbooru.donmai.us/posts/*
// @icon        data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4KPHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAxNiAxNiIgaGVpZ2h0PSIxNiIgd2lkdGg9IjE2IiB2ZXJzaW9uPSIxLjEiPgoJPGRlZnM+CgkJPGxpbmVhckdyYWRpZW50IGlkPSJiZyIgZ3JhZGllbnRUcmFuc2Zvcm09InJvdGF0ZSg4NSkiPgoJCQk8c3RvcCBvZmZzZXQ9IjQ5JSIgc3RvcC1jb2xvcj0iI2JhOTU3MCIgLz4KCQkJPHN0b3Agb2Zmc2V0PSI2NyUiIHN0b3AtY29sb3I9IiNhNDgxNWYiIC8+CgkJPC9saW5lYXJHcmFkaWVudD4KCTwvZGVmcz4KCTxnPgoJCTxwYXRoIGQ9Ik0gMS41LDE0LjUgViA0LjI1IEwgNC4yNSwxLjUgSCAxNC41IHYgMTAuMjUgbCAtMi43NSwyLjc1IHoiIGZpbGw9InVybCgjYmcpIiBzdHJva2U9ImJsYWNrIiBzdHJva2Utd2lkdGg9IjEiIC8+CgkJPHBhdGggZD0ibSAxLjUsNC41IGggMTAgdiAxMCIgc3Ryb2tlPSJibGFjayIgc3Ryb2tlLXdpZHRoPSIxIiBmaWxsPSJub25lIiAvPgoJCTxwYXRoIGQ9Im0gMTQuNSwxLjUgLTMsMyIgc3Ryb2tlPSJibGFjayIgc3Ryb2tlLXdpZHRoPSIxIiAvPgoJPC9nPgo8L3N2Zz4K
// @grant       clipboardWrite
// @grant       GM_setClipboard
// @grant       GM_xmlhttpRequest
// @require     https://raw.githubusercontent.com/usadev1984/chlsdl-modules/2afa7cac6623ac0b828a1893ebda93c18cd96f40/js-scripts/chlsdl-common/index.js
// @version     1.0
// @author      usadev1984
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
        let url = document.querySelector('#image > video')
        if (url == null) {
            url = document.querySelector('#content > #image-resize-notice > a[href]')
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
        } else {
            /*
             * looking for this element to get the video url has the added benefit
             * of allowing us to get a name for the file that's consistent with the
             * names of images
             */
            url = document.querySelector('#post-option-download > a')
            if (url == null) {
                alert("url not found")
                return
            }
            url = url.href;
            console.log(url)
        }

        // let name = url.match(/(?:.+)\/(.*)/)
        /*
         * disgusting. TODO: use `URL()` to parse url
         */
        let name = url.match(/(?:.+\/)(.+)(?:(\..+?)(?:\?.+)|(\..+))/)
        name = name.splice(1, name.length - 1).join('')

        let source = window.location.href
        console.log(url + '\n' + source);
        source = source.replace(/\?.*/, '')

        let post_info = get_post_info();

        var postlist = document.querySelectorAll('article[id^="post_"]');
        var parent_id = null
        var children = []
        var siblings = []
        if (!(postlist == null) && postlist.length != 0) {
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

        chlsdl_send_to_chlsdl(url, {
            "url": url,
            "name": name,
            "source": source,
            "information": post_info,
            "parent_id": parent_id,
            "children": children,
            "siblings": siblings,
        });
    }

    function get_taglist(list_class) {
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
