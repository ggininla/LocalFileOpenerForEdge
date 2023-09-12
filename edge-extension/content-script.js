document.body.addEventListener('click', e => {
    if (!e.isTrusted) {
        return;
    }

    //  Finds the closest A node
    var el = undefined;
    for (el = e.target; el.tagName && el.tagName.toLowerCase() !== 'a'; el = el.parentNode);

    // Nothing found..
    if (!el) return;

    // Interceppts targets starting with URI "file://"
    if (el.href && typeof el.href === 'string' && el.href.startsWith('file://')) {
        // Stops event propagation
        e.preventDefault();
        chrome.runtime.sendMessage({pathToFile: el.href.substr('file://'.length)});
    }
});