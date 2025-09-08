// ==UserScript==
// @name         BLOCK YOUTBE
// @namespace    http://tampermonkey.net/
// @version      0.1
// @description  This script blocks the youtube and redirects to a set url
// @author       You
// @match        https://www.youtube.com/*
// @icon         https://www.google.com/s2/favicons?sz=64&domain=youtube.com
// @grant        window.onurlchange
// ==/UserScript==

/*
This script blocks the youtube homepage.

note: 
  The way onurlchange works is stupid, the event triggers before you can
  read the new url.
  create a timeout function and wait for the new url to become readable.
*/


var CUR_TIMEOUT_ID = null;
let REDIRECT_URL = "http://192.168.0.189:8000/static/index.html";


function checkOnYoutube() {
    if (window.location.href != undefined) {
        let href = window.location.href;
        if (href.startsWith('https://www.youtube.com')) { window.location.href = REDIRECT_URL; }
        if (href == 'https://www.youtube.com/') { window.location.href = REDIRECT_URL; }
        if (href == 'https://www.youtube.com') { window.location.href = REDIRECT_URL; }

        return true;
    }
    else {
        return false;
    }
}


function urlChangeStandby() {
    console.log('waiting for url....');
    if (window.location.href != undefined && checkOnYoutube()) { // has a second check to make sure we have the url just in case
        if (CUR_TIMEOUT_ID != null) {
            clearTimeout(CUR_TIMEOUT_ID);
            CUR_TIMEOUT_ID = null;
        }
    }
}

// Function to handle URL changes
function handleUrlChange(newUrl) {
    CUR_TIMEOUT_ID = setTimeout(urlChangeStandby(), 10);
}

// Listen for URL changes
window.addEventListener('urlchange', function(event) {
    handleUrlChange(event.detail);
});

// Initial call to handle the current URL
handleUrlChange(window.location.href);

