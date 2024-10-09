/********************************************************************************
 * Copyright (C) 2020 EclipseSource and others.
 *
 * This program and the accompanying materials are made available under the
 * terms of the MIT License, which is available in the project root.
 *
 * SPDX-License-Identifier: MIT
 ********************************************************************************/

import { Key, KeyCode } from '@theia/core/lib/browser';
import { nls } from '@theia/core';
import { WindowService } from '@theia/core/lib/browser/window/window-service';
import { CommandService } from '@theia/core/lib/common/command';
import { GenerateExampleCommand, CdtCloudBlueprintExamples } from '@eclipse-cdt-cloud/blueprint-examples/lib/browser';
import * as React from 'react';

export interface ExternalBrowserLinkProps {
    text: string;
    url: string;
    windowService: WindowService;
}

function ExternalBrowserLink(props: ExternalBrowserLinkProps): JSX.Element {
    return <a
        role={'button'}
        tabIndex={0}
        onClick={() => openExternalLink(props.url, props.windowService)}
        onKeyDown={(e: React.KeyboardEvent) => {
            if (Key.ENTER.keyCode === KeyCode.createKeyCode(e.nativeEvent).key?.keyCode) {
                openExternalLink(props.url, props.windowService);
            }
        }}>
        {props.text}
    </a>;
}

function openExternalLink(url: string, windowService: WindowService): void {
    windowService.openNewWindow(url, { external: true });
}

export function renderWhatIs(windowService: WindowService, commandService: CommandService): React.ReactNode {
    return <div className='gs-section'>
        <h3 className='gs-section-header'>
            Welcome to CDT Cloud Blueprint
        </h3>
        <div >
            CDT Cloud Blueprint is a <span className='gs-text-bold'>template</span> tool for building custom, web-based C/C++ tools.
            Purely based on extensible open source components, it offers a modern and feature-rich C/C++ development experience,
            including language editing and debugging support, memory debugging and a tracing view.
            It is meant to serve as a starting point for the implementation of your own domain-specific custom C/C++ tool.
        </div>
        <h3 className='gs-section-header'>
            Start playing around!
        </h3>
        <div>Select and generate an <a
                role={'button'}
                tabIndex={0}
                onClick={() => generateExample(commandService)}
                onKeyDown={(e: React.KeyboardEvent) => generateExample(commandService)}>
                {'example project'}
            </a>.</div>
        <div>Explore the features, such as code editing, building, build configurations, debugging, etc.</div>
        <div>
            <a
                role={'button'}
                tabIndex={0}
                onClick={() => generateExample(commandService, CdtCloudBlueprintExamples.EXAMPLE_TRACES)}>
                {nls.localizeByDefault('Generate example traces')}
            </a> {' '} and open them with Trace Compass Cloud.</div>
    </div>;
}

function generateExample(commandService: CommandService, exampleId?: string): void {
    commandService.executeCommand(GenerateExampleCommand.id, exampleId);
}

export function renderWhatIsNot(): React.ReactNode {
    return <div className='gs-section'>
        <h3 className='gs-section-header'>
            What CDT Cloud Blueprint isn't
        </h3>
        <div >
            CDT Cloud Blueprint is <span className='gs-text-bold'>not meant to be used as a production-ready product</span>.
            However, feel free to use it a template for building your own custom C/C++ tool based on this blueprint, as well
            as for testing the integrated CDT Cloud components.
        </div>
    </div>;
}

export function renderSupport(windowService: WindowService): React.ReactNode {
    return <div className='gs-section'>
        <h3 className='gs-section-header'>
            Professional Support
        </h3>
        <div >
            Professional support, implementation services, consulting and training for building tools like this instance of CDT Cloud Blueprint and for
            building other tools based on Eclipse Theia is available by selected companies as listed on
            the <ExternalBrowserLink text="CDT Cloud support page" url="https://www.eclipse.org/cdt-cloud/support/" windowService={windowService} ></ExternalBrowserLink>.
        </div>
    </div>;
}

export function renderTickets(windowService: WindowService): React.ReactNode {
    return <div className='gs-section'>
        <h3 className='gs-section-header'>
            Get involved with CDT Cloud
        </h3>
        <div >
            CDT Cloud Blueprint is part of the CDT Cloud project, which hosts components and best practices for building
            customizable web-based C/C++ tools. For more information on CDT Cloud visit us
            on <ExternalBrowserLink text="our webpage" url="https://www.eclipse.org/cdt-cloud"
                windowService={windowService} ></ExternalBrowserLink> or
            on <ExternalBrowserLink text="Github" url="https://github.com/eclipse-cdt-cloud/cdt-cloud"
                windowService={windowService} ></ExternalBrowserLink> and <ExternalBrowserLink text="get in touch"
                    url="https://www.eclipse.org/cdt-cloud/contact" windowService={windowService} ></ExternalBrowserLink> to
            discuss ideas, request features, report bugs, or to get support for building your custom C/C++ tool.
        </div>
    </div>;
}

export function renderSourceCode(windowService: WindowService): React.ReactNode {
    return <div className='gs-section'>
        <h3 className='gs-section-header'>
            Source Code and CDT Cloud components
        </h3>
        <div >
            The source code of CDT Cloud Blueprint is available
            on <ExternalBrowserLink text="Github" url="https://github.com/eclipse-cdt-cloud/cdt-cloud-blueprint"
                windowService={windowService} ></ExternalBrowserLink>.
        </div>
        <div >
            CDT Cloud Blueprint bundles the following CDT Cloud open-source components:
            <ul>
                <li>
                    <ExternalBrowserLink text="CDT GDB Debug Adapter" url="https://github.com/eclipse-cdt-cloud/cdt-gdb-adapter"
                        windowService={windowService} ></ExternalBrowserLink>
                </li>
                <li>
                    <ExternalBrowserLink text="Clangd Contexts" url="https://github.com/eclipse-cdt-cloud/clangd-contexts"
                        windowService={windowService} ></ExternalBrowserLink>
                </li>
                <li>
                    <ExternalBrowserLink text="Trace Compass Cloud" url="https://github.com/eclipse-cdt-cloud/theia-trace-extension"
                        windowService={windowService} ></ExternalBrowserLink>
                </li>
            </ul>
        </div>
    </div>;
}

export function renderDocumentation(windowService: WindowService): React.ReactNode {
    return <div className='gs-section'></div>;
}

export function renderDownloads(): React.ReactNode {
    return <div className='gs-section'>
        <h3 className='gs-section-header'>
            Updates and aaa
        </h3>
        <div className='gs-action-container'>
            You can update CDT Cloud Blueprint directly in this application by navigating to
            File {'>'} Settings {'>'} Check for Updates. Moreover the application will check for Updates
            after each launch automatically.
            Alternatively you can download the most recent version from our webpage.
        </div>
    </div>;
}

export function descContent(): React.ReactNode {
    return <div>
        <hr className='gs-hr' />
        <h2>什么是手持终端?</h2>
        <p>用于构建手持移动终端模拟开发、远程调试的开发测试环境，能够模拟实装环境下的移动硬件设备和操作系统。<br/>
            包括手持终端微内核框架、手持终端软件开发模拟器、手持移动终端远程调试器。</p>
                
        <hr className='gs-hr' />
        <h2>手持终端微内核框架</h2>
        <p>手持终端微内核框架提供面向终端的服务资源管理和内核管理功能。服务资源管理包括输入资源、输出资源、计算<br/>
            资源、存储资源、网络资源、占用内存资源、CPU大小资源。内核管理功能包括服务支撑、进程通信、数据访问、<br/>
            界面展现和安全保密。</p>

        <hr className='gs-hr' />
        <h2>手持终端软件开发模拟器</h2>
        <p>提供手持终端软件开发模拟器，主要功能模块包括模拟工具、调试工具和部署工具。提供与真实设备相同的功能和<br/>
            接口，能够对手持终端的系统、计算、存储资源进行模拟，并提供对应图形化界面在计算机上运行，能够在没有<br/>
            真实硬件的情况下开发测试应用程序。</p>

        <hr className='gs-hr' />
        <h2>手持移动终端远程调试器</h2>
        <p>手持移动终端远程调试器提供真机和本地端两种调试手段，真机远程调试能够提供网页真机调试、常用操作模拟、<br/>
            操作快照、自主截图功能。本地端调试器主要提供移动版软件在设备真机上的调试服务；支持远程操纵，触摸<br/>
            使用，操纵响应时间不大于0.5s。手持移动终端远程调试器功能包括本地调试和远程调试。</p>
    </div>
}