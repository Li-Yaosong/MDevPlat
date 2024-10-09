
// utils.ts
import { useEffect, useState } from 'react';
export const isBrowser = (): boolean => typeof window !== 'undefined';

export const getCurrentUrl = (): string => {
    const [currentUrl, setCurrentUrl] = useState('');
    useEffect(() => {
        isBrowser() ? setCurrentUrl(window.location.href) : '';
    }, []);
    return currentUrl;
  };
  
  export const getHostname = (): string => {
    const [hostname, setHostname] = useState('');
    useEffect(() => {
        isBrowser() ? setHostname(window.location.hostname) : '';
    }, []);
    return hostname;
  };
  
  export const getPathname = (): string => {
    const [pathname, setPathname] = useState('');
    useEffect(() => {
        isBrowser() ? setPathname(window.location.pathname) : '';
    }, []);
    return pathname;
  };
