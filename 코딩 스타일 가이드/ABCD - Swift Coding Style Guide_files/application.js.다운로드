// This source file is part of the Swift.org open source project
//
// Copyright (c) 2014 - 2018 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors

/*! gumshoe v3.0.0 | (c) 2015 Chris Ferdinandi | MIT License | http://github.com/cferdinandi/gumshoe */
!function(e,t){"function"==typeof define&&define.amd?define([],t(e)):"object"==typeof exports?module.exports=t(e):e.gumshoe=t(e)}("undefined"!=typeof global?global:this.window||this.global,function(e){"use strict";var t,n,a,o,r,c,s={},i="querySelector"in document&&"addEventListener"in e&&"classList"in document.createElement("_"),l=[],u={selector:"[data-gumshoe] a",selectorHeader:"[data-gumshoe-header]",offset:0,activeClass:"active",callback:function(){}},f=function(e,t,n){if("[object Object]"===Object.prototype.toString.call(e))for(var a in e)Object.prototype.hasOwnProperty.call(e,a)&&t.call(n,e[a],a,e);else for(var o=0,r=e.length;r>o;o++)t.call(n,e[o],o,e)},d=function(){var e={},t=!1,n=0,a=arguments.length;"[object Boolean]"===Object.prototype.toString.call(arguments[0])&&(t=arguments[0],n++);for(var o=function(n){for(var a in n)Object.prototype.hasOwnProperty.call(n,a)&&(t&&"[object Object]"===Object.prototype.toString.call(n[a])?e[a]=d(!0,e[a],n[a]):e[a]=n[a])};a>n;n++){var r=arguments[n];o(r)}return e},v=function(e){return Math.max(e.scrollHeight,e.offsetHeight,e.clientHeight)},g=function(){return Math.max(document.body.scrollHeight,document.documentElement.scrollHeight,document.body.offsetHeight,document.documentElement.offsetHeight,document.body.clientHeight,document.documentElement.clientHeight)},m=function(e){var n=0;if(e.offsetParent)do n+=e.offsetTop,e=e.offsetParent;while(e);return n=n-r-t.offset,n>=0?n:0},p=function(){l.sort(function(e,t){return e.distance>t.distance?-1:e.distance<t.distance?1:0})};s.setDistances=function(){a=g(),r=o?v(o)+m(o):0,f(l,function(e){e.distance=m(e.target)}),p()};var h=function(){var e=document.querySelectorAll(t.selector);f(e,function(e){e.hash&&l.push({nav:e,target:document.querySelector(e.hash),parent:"li"===e.parentNode.tagName.toLowerCase()?e.parentNode:null,distance:0})})},y=function(e){c&&(c.nav.classList.remove(t.activeClass),c.parent&&c.parent.classList.remove(t.activeClass)),e.nav.classList.add(t.activeClass),e.parent&&e.parent.classList.add(t.activeClass),t.callback(e),c={nav:e.nav,parent:e.parent}};s.getCurrentNav=function(){var t=e.pageYOffset;if(e.innerHeight+t>=a)return y(l[0]);for(var n=0,o=l.length;o>n;n++){var r=l[n];if(r.distance<t)return y(r)}};var b=function(){f(l,function(e){e.nav.classList.contains(t.activeClass)&&(c={nav:e.nav,parent:e.parent})})};s.destroy=function(){t&&(e.removeEventListener("resize",H,!1),e.removeEventListener("scroll",H,!1),l=[],t=null,n=null,a=null,o=null,r=null,c=null)};var H=function(e){n||(n=setTimeout(function(){n=null,"scroll"===e.type&&s.getCurrentNav(),"resize"===e.type&&(s.setDistances(),s.getCurrentNav())},66))};return s.init=function(n){i&&(s.destroy(),t=d(u,n||{}),o=document.querySelector(t.selectorHeader),h(),0!==l.length&&(b(),s.setDistances(),s.getCurrentNav(),e.addEventListener("resize",H,!1),e.addEventListener("scroll",H,!1)))},s});


(function() {
    gumshoe.init({
        selector: 'nav[role="navigation"] > ul li.active ul li a',
        selectorHeader: 'nav[role="navigation"] > ul li.active',
        offset: 0,
        activeClass: 'active',
        callback: function (nav) {
            var title = document.title + " - " + nav.target.textContent
            var hash = "#" + nav.target.id;
            if (window.location.hash !== hash) {
                history.replaceState(null, title, window.location.pathname + hash);
            }
        }
    });

    function toggleClass(element, className){
        if (!element || !className){
            return;
        }

        var classString = element.className, nameIndex = classString.indexOf(className);
        if (nameIndex == -1) {
            classString += ' ' + className;
        } else {
            classString = classString.substr(0, nameIndex) + classString.substr(nameIndex+className.length);
        }
        element.className = classString;
    }

    document.getElementById('menu-toggle').addEventListener('mousedown', function() {
        toggleClass(document.getElementById('menu-toggle'), 'open');
        toggleClass(document.querySelector('nav[role="navigation"]'), 'open');
    });
})();

