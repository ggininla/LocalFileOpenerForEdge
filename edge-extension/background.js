/**
 * Copyright (c) 2023, Auckland Transport
 * Computer Vision Team
 * 
 * The service script listens on handleFileUri messages and invokes native host
 * app "FileExplorerLauncher", which will start File Explorer in the location
 * specified in the message (e.g. "file://C:/Path/To/A/Directory")
 */

'use strict';

const MESSAGE_METHOD = 'handleFileUri';
const CONTENT_SCRIPT = 'content-script.js';
const PAGE_NOT_LOADED = chrome.i18n.getMessage("page_not_loaded");
const HOST_APP = 'nz.govt.at.file_explorer_launcher';

const runContentScriptOnTabs = (tabs, script) => {
    const onResult = (result) => {
        if (typeof result == "undefined") {
            console.error(PAGE_NOT_LOADED, tab);
        }
    }

    const runOnTab = (tab) => {
        chrome.scripting.executeScript({
            target: { tabId: tab.id, allFrames: true },
            files: [script]
        }).then(onResult)
    }

    tabs.forEach(runOnTab);
}

chrome.runtime.onInstalled.addListener(() =>
    chrome.tabs.query({ url: ['http://*/*', 'https://*/*', 'file://*'] }, tabs => runContentScriptOnTabs(tabs, CONTENT_SCRIPT))
);

chrome.runtime.onMessage.addListener((message, sender) => {
    if (!message.pathToFile) {
        console.error(`Invalid message: ${message}`)
        return;
    }

    const toUncPath = (path) => {
        const fqdnPattern = /(?=^.{4,253}$)(^((?!-)[a-zA-Z0-9-]{0,62}[a-zA-Z0-9]\.)+[a-zA-Z]{2,63}$)/gm
        const pathSplitter = (path) => path.split('/').filter(x => x !== '');
        const parts = pathSplitter(path);

        // FQDN Path
        if (parts[0].match(fqdnPattern)) return '//' + parts.join('/');
        
        // Path with drive letter
        if (parts[0].includes(':')) return parts.join('/');

        // Relative path
        if (sender.origin !== 'file://') {
            console.error('Unable to calculate full UNC path');
            return parts.join('/');
        }

        // Performs path concatenation
        return [...pathSplitter(sender.url.substr('file://'.length)).slice(0, -1), ...parts].join('/');
    }

    chrome.runtime.sendNativeMessage(
        HOST_APP,
        {path: toUncPath(message.pathToFile)},
        (response) => console.log('Received ' + response)
    );
});
