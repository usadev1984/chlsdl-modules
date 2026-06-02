// ==UserScript==
// @name        chlsdl-bunkr
// @namespace   Violentmonkey Scripts
// @match       https://bunkr.si/f/*
// @icon        data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAHL0lEQVRYR71XB4xUVRQ9f3pd6spSd4OAIiAgbCgxgCggoBSxYgSEBMQCUiwR1Fgwgg0Vo2IBo6HKIlhiUAEjgoAIKAvSpC19YZfZ6X/+/97zxtFdXZwlIdxsMrOT/9+779xzzr1PmzOi3DqxzwAsXNrQgFqX2aBN737GgvzjdGuw2S5NDpYcVo9bME1IAt3OWC6fhhsf9KFBCzss/igJ8aGLGWpNWVA+ECo18cUrUYTPmtCmSQLegIYRrwbRtK0Dhm4hGrLgy7HB7rw4KZhS4Ui5CY9fg9OjofyEiffHh9RnlQQat3Zg1dtR7FidRNveLtww1guHizlXH1yYoUp3nseI6I+L4thUFEd+ewcGTvYjXmHhPUngXOUERr4WRK0GNrw7NoTSwyZy820Y81YOauf9QwxDB04fNHCkOIWTB1KIlluqZN4cDfWb2RWCDS63wyWnzEQiYmHexAoc3JZCoK6GMXOC8NWyYe59/0qAJcggULwmiTa9BIFxaQRIliO/pbBpeRz7f06p2hkpS06e3sg05Ltdg6+2hvx2ThQOdaNFoVOVUCGwMI6N8m5BeycGTvIhHq4GgQwH9ISFSJkFfx2bKANIxixsLErIIjGETllqUaISqGvD8b0p2RzIE/ImY8DZowb0BOCvpaHzIDd6jPDKaTVJFggL8YgUCV92XDhwfzUIEMLKwc1Xvx/DhqVxtUiewNvpZjeuuNaF6DkTH0+pQDJq4Y4ZQTRqZce+TTq2rEyoElFFV/dxYcDDfvgFmcpRowQI3boFMXw7NybKoE9AFvOhcIhHrXVyv4EPJ4SQlBrfPTOIFl3Sktm9XseyZ8OSoNiL0KfrrR70e6AqmWuUwKHtOhZOiyAi9SYPWJp6TW0Y9IgfBR2cslESRTMi6veBE33oOMCN04cNrJwZRcmuVJo7whN+3jLNj7bXu/4GIWsCPHHRjDC2fpVEoyvt6hQ/fBLH6UOGYjsVUrIzJdoWnOXPIz7S5CoHwmUmju8xEKxvQ69RHuz8Xse+jTouL3Rg+ItBeIPpUmRN4MReA/MnVQgZTQV7t9s8wn4dK2ZGcPaIqepLMubkipcLzBVCsGRcrEC+U2oDBJH2fd3YuTaJpc9ElEcMfyGAll3TZcqawKbPElg5K4I6DW0Y9XoQ9ZoIy8W7F00L4/d1umoiPUZ40KqbU8nv0DYda+bFUHrIFCnblatSJeTBR5MrlISvG+1Fn/u8NUvgi1ejSnas9U1TfMo+S6W+K16MICYu1n9CGpXKQe8gJyhLaj3/aof6ThLvWJ0QDrhx53MBhdz/IsCXPn0mjO2rkmpj+jeDPSImBlJb3PLeNwSVpvYqCfC08yeGcHS3AfaWjIVTygmRakEHB+i2Lm8WH2ACSyWBXyUBZmv7ax/KkuSs09iG0W/koE6jqr2b7jZfLJce4BDCkw8MvpdKQvWBUbNrkABf+vylCNYvTaBpGzt6j/bBIR5wRsj3zTtRWczCkMf96NBffqwU+zfrWDQ9rJyz1yiv6gvcfP3iOHb/qKN1TyfumhFQyGQl4YYlcZAHuQV2lTWtl01lwRNh7P1JVzLsO96H5p1IQijds4uWFBvIa2nHSCEhFcJ3Pp5agf1bUuhxjwf9H/LVjISEkTYblwV42msGunFM9F30fFjpnEFu0JiogjMlhmI8g8rhO3RGWvNCUY4hqN3+bABX9UybUVYEOC4teTqM4jU6Cjo60HOkF2s+iOHIjpQymZzLNGnLon2Bm8Ga5+bbVZcrO2Yqs+r3oBfbv07it++SaNbOgXteDqgGV6ME+NCeDbpKgs3GLayOyQmp7f4TvWgppyteq+PrN9OcYM07D3bj1B9ixcKfUuGLT1yP3sG4eapfdcdMZEWAD7L7fSukWye9nGQiETk30pYZ52tGdL/lL6T9gtt36OfGoEd9cEsbvqAE+DDr+uXsiJIk7bT5NU50GeZWn+UnTcxjNxSEhks3pPEcVkNLQlifREokS6SGPOZHbeFF5chqROQATYgMZ8NZOz+GLZ8n1CznFvI1aG5XXKD0OBlRDXznuPSQmAy0TnHctte50GecT21OBFkOStAuI8d5E2jSxqHGJzWUygLd7/DAJi/QSHiqjcsSqgvGw+mGRAUwLDPdFZ3icnktHSiUere7waWSZfzyVQKbBRkSuvcYr5q4qh9KRe9zx4UUw3MLZCidU3UoJctLRKIHtuo4dcBUUxETyQyl+cL2ZlIKkjUTtGE65IGtoqB6GkZzKJWRv9qhtKGMVStmRbHr+yRa93AJe33Ku6sLkpT9gUGUHA55rppHVUN6N4rN0mHpjsOeCqhy/QeBzFCa1rIhhmKH568BotoMLuBH+sXZElN4oykvqHIx4dWMtRoqYxPHclVbXqOEOBfzdsbLS+a6d04UtPjJsLqiqcsp7228LFysq1g2cFiWqFzV1OV01uAyK3QqTaZLGWzXNKc/ASZBYIKUttOtAAAAAElFTkSuQmCC
// @grant       clipboardWrite
// @grant       GM_setClipboard
// @grant       GM_xmlhttpRequest
// @require     https://raw.githubusercontent.com/usadev1984/chlsdl-modules/2afa7cac6623ac0b828a1893ebda93c18cd96f40/js-scripts/chlsdl-common/index.js
// @version     1.0
// @author      usadev1984
// @description 6/2/2026, 09:57:00 AM
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
        // let url = document.querySelector('#player > source');
        // if (url == null) {
        //     url = document.querySelector('#img-enlarge'); // document.querySelector('a[class*="btn"][class*="ic-download"]')
        //     if (url == null) {
        //         url = document.querySelector('a[class*="btn"][class*="ic-download"]')
        //     }
        //     url = url.href;
        // } else
        //     url = url.src;
        let url = document.querySelector('a[class*="btn"][class*="ic-download"]');
        if (url == null) {
            alert('url not found"' + url + '"');
            return;
        } else
            url = url.href;

        console.log(url)

        let name = document.querySelector('h1').innerText

        let source = window.location.href
        console.log(url + '\n' + source);
        source = source.replace(/\?.*/, '')

        chlsdl_send_to_chlsdl(url, {
            "url": url,
            "name": name,
            "source": source,
        });
    }
})();
