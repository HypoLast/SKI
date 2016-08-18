type Nullable<T> = T | null;

declare module 'readline-sync' {
    export interface Options {
        prompt?:string;
        hideEchoBack?:boolean;
        mask?:string;
        limit?:string|number|RegExp|((i:string)=>boolean)|string[];
        limitMessage?:string;
        defaultInput?:string;
        trueValue?:string|number|RegExp|((i:string)=>boolean)|string[];
        falseValue?:string|number|RegExp|((i:string)=>boolean)|string[];
        caseSensitive?:boolean;
        keepWhitespace?:boolean;
        encoding?:string;
        bufferSize?:number;
        print?:Nullable<(m:string, encoding:string)=>void>;
        history?:boolean;
        cd?:boolean;
    }

    export function setDefaultOptions(options?:Options):Options;
    export function prompt(options?:Options):string;
    export function question(query?:string, options?:Options):string;
    export function keyIn(query?:string, options?:Options):string;
    export function keyInPause(query?:string, options?:Options):void;

    // ... and others
}