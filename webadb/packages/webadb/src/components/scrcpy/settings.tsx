import {
    Dropdown,
    IDropdownOption,
    Icon,
    IconButton,
    Position,
    SpinButton,
    Stack,
    TextField,
    Toggle,
    TooltipHost,
} from "@fluentui/react";
import { makeStyles } from "@griffel/react";
import { AdbSyncError } from "@yume-chan/adb";
import { AdbScrcpyClient, AdbScrcpyOptionsLatest } from "@yume-chan/adb-scrcpy";
// import { VERSION } from "@yume-chan/fetch-scrcpy-server";
import {
    CodecOptions,
    DEFAULT_SERVER_PATH,
    ScrcpyDisplay,
    ScrcpyEncoder,
    ScrcpyLogLevel,
    ScrcpyOptionsInitLatest,
    ScrcpyOptionsLatest,
    ScrcpyVideoOrientation,
} from "@yume-chan/scrcpy";
import {
    ScrcpyVideoDecoderConstructor,
    TinyH264Decoder,
} from "@yume-chan/scrcpy-decoder-tinyh264";
import { ConcatStringStream, DecodeUtf8Stream } from "@yume-chan/stream-extra";
import {
    autorun,
    computed,
    makeAutoObservable,
    observable,
    runInAction,
} from "mobx";
import { observer } from "mobx-react-lite";
import { GLOBAL_STATE } from "../../state";
import { Icons } from "../../utils";
import { STATE } from "./state";

const VERSION = '2.6.1';

export type Settings = ScrcpyOptionsInitLatest;

export interface ClientSettings {
    turnScreenOff?: boolean;
    decoder?: string;
    ignoreDecoderCodecArgs?: boolean;
}

export type SettingKeys = keyof (Settings & ClientSettings);

export interface SettingDefinitionBase {
    group: "settings" | "clientSettings";
    key: SettingKeys;
    type: string;
    label: string;
    labelExtra?: JSX.Element;
    description?: string;
}

export interface TextSettingDefinition extends SettingDefinitionBase {
    type: "text";
    placeholder?: string;
}

export interface DropdownSettingDefinition extends SettingDefinitionBase {
    type: "dropdown";
    placeholder?: string;
    options: IDropdownOption[];
}

export interface ToggleSettingDefinition extends SettingDefinitionBase {
    type: "toggle";
    disabled?: boolean;
}

export interface NumberSettingDefinition extends SettingDefinitionBase {
    type: "number";
    min?: number;
    max?: number;
    step?: number;
}

export type SettingDefinition =
    | TextSettingDefinition
    | DropdownSettingDefinition
    | ToggleSettingDefinition
    | NumberSettingDefinition;

interface SettingItemProps {
    definition: SettingDefinition;
    value: any;
    onChange: (definition: SettingDefinition, value: any) => void;
}

const useClasses = makeStyles({
    labelRight: {
        marginLeft: "4px",
    },
    item: {
        width: "100%",
        maxWidth: "300px",
    },
});

export const SettingItem = observer(function SettingItem({
    definition,
    value,
    onChange,
}: SettingItemProps) {
    const classes = useClasses();

    let label: JSX.Element = (
        <Stack horizontal verticalAlign="center">
            <span>{definition.label}</span>
            {!!definition.description && (
                <TooltipHost content={definition.description}>
                    <Icon
                        className={classes.labelRight}
                        iconName={Icons.Info}
                    />
                </TooltipHost>
            )}
            {definition.labelExtra}
        </Stack>
    );

    switch (definition.type) {
        case "text":
            return (
                <TextField
                    className={classes.item}
                    label={label as any}
                    placeholder={definition.placeholder}
                    value={value}
                    onChange={(e, value) => onChange(definition, value)}
                />
            );
        case "dropdown":
            return (
                <Dropdown
                    className={classes.item}
                    label={label as any}
                    options={definition.options}
                    placeholder={definition.placeholder}
                    selectedKey={value}
                    onChange={(e, option) => onChange(definition, option!.key)}
                />
            );
        case "toggle":
            return (
                <Toggle
                    label={label}
                    checked={value}
                    disabled={definition.disabled}
                    onChange={(e, checked) => onChange(definition, checked)}
                />
            );
        case "number":
            return (
                <SpinButton
                    className={classes.item}
                    label={definition.label}
                    labelPosition={Position.top}
                    min={definition.min}
                    max={definition.max}
                    step={definition.step}
                    value={value.toString()}
                    onChange={(e, value) =>
                        onChange(definition, Number.parseInt(value!, 10))
                    }
                />
            );
    }
});

export interface DecoderDefinition {
    key: string;
    name: string;
    Constructor: ScrcpyVideoDecoderConstructor;
}

const DEFAULT_SETTINGS = {
    maxSize: 1080,
    videoBitRate: 4_000_000,
    videoCodec: "h264",
    lockVideoOrientation: ScrcpyVideoOrientation.Unlocked,
    displayId: 0,
    crop: "",
    powerOn: true,
    audio: true,
    audioCodec: "aac",
} as Settings;

export const SETTING_STATE = makeAutoObservable(
    {
        displays: [] as ScrcpyDisplay[],
        encoders: [] as ScrcpyEncoder[],
        decoders: [
            {
                key: "tinyh264",
                name: "TinyH264 (Software)",
                Constructor: TinyH264Decoder,
            },
        ] as DecoderDefinition[],

        settings: DEFAULT_SETTINGS,

        clientSettings: {} as ClientSettings,
    },
    {
        decoders: observable.shallow,
        settings: observable.deep,
        clientSettings: observable.deep,
    }
);

export const SCRCPY_SETTINGS_FILENAME = "/data/local/tmp/.tango.json";

autorun(() => {
    if (GLOBAL_STATE.adb) {
        (async () => {
            const sync = await GLOBAL_STATE.adb!.sync();
            try {
                const content = await sync
                    .read(SCRCPY_SETTINGS_FILENAME)
                    .pipeThrough(new DecodeUtf8Stream())
                    .pipeThrough(new ConcatStringStream());
                const settings = JSON.parse(content);
                settings.settings.videoCodecOptions = new CodecOptions(
                    settings.settings.videoCodecOptions,
                );
                settings.settings.audioCodecOptions = new CodecOptions(
                    settings.settings.audioCodecOptions,
                );
                runInAction(() => {
                    SETTING_STATE.settings = {
                        ...DEFAULT_SETTINGS,
                        ...settings.settings,
                    };
                    SETTING_STATE.clientSettings = settings.clientSettings;
                });
            } catch (e) {
                if (!(e instanceof AdbSyncError)) {
                    throw e;
                }
            } finally {
                await sync.dispose();
            }
        })();

        runInAction(() => {
            SETTING_STATE.encoders = [];
            SETTING_STATE.displays = [];
            SETTING_STATE.settings.displayId = undefined;
        });
    }
});

autorun(() => {
    SETTING_STATE.clientSettings.decoder = SETTING_STATE.decoders[0].key;
});

export const SETTING_DEFINITIONS = computed(() => {
    const result: SettingDefinition[] = [];

    result.push(
        {
            group: "settings",
            key: "powerOn",
            type: "toggle",
            label: "启动时唤醒设备",
        },
        {
            group: "clientSettings",
            key: "turnScreenOff",
            type: "toggle",
            label: "镜像期间关闭屏幕",
        },
        {
            group: "settings",
            key: "stayAwake",
            type: "toggle",
            label: "镜像期间保持唤醒状态（如果已插入）",
        },
        {
            group: "settings",
            key: "powerOffOnClose",
            type: "toggle",
            label: "停止时关闭设备",
        }
    );

    result.push({
        group: "settings",
        key: "displayId",
        type: "dropdown",
        label: "展示",
        placeholder: "按刷新来更新可用的显示",
        labelExtra: (
            <IconButton
                iconProps={{ iconName: Icons.ArrowClockwise }}
                disabled={!GLOBAL_STATE.adb}
                text="刷新"
                onClick={async () => {
                    try {
                        await STATE.pushServer();

                        const displays = await AdbScrcpyClient.getDisplays(
                            GLOBAL_STATE.adb!,
                            DEFAULT_SERVER_PATH,
                            VERSION,
                            new AdbScrcpyOptionsLatest(
                                new ScrcpyOptionsLatest({
                                    logLevel: ScrcpyLogLevel.Debug,
                                })
                            )
                        );

                        runInAction(() => {
                            SETTING_STATE.displays = displays;
                            if (
                                !SETTING_STATE.settings.displayId ||
                                !SETTING_STATE.displays.some(
                                    (x) =>
                                        x.id ===
                                        SETTING_STATE.settings.displayId
                                )
                            ) {
                                SETTING_STATE.settings.displayId =
                                    SETTING_STATE.displays[0]?.id;
                            }
                        });
                    } catch (e: any) {
                        GLOBAL_STATE.showErrorDialog(e);
                    }
                }}
            />
        ),
        options: SETTING_STATE.displays.map((item) => ({
            key: item.id,
            text: `${item.id}${item.resolution ? ` (${item.resolution})` : ""}`,
        })),
    });

    result.push({
        group: "settings",
        key: "crop",
        type: "text",
        label: "Crop",
        placeholder: "W:H:X:Y",
    });

    result.push(
        {
            group: "settings",
            key: "maxSize",
            type: "number",
            label: "最大分辨率（长边，0 = 无限制）",
            min: 0,
            max: 2560,
            step: 50,
        },
        {
            group: "settings",
            key: "videoBitRate",
            type: "number",
            label: "最大视频比特率 (bps)",
            min: 100,
            max: 100_000_000,
            step: 100,
        },
        {
            group: "settings",
            key: "videoCodec",
            type: "dropdown",
            label: "视频编解码器",
            options: [
                {
                    key: "h264",
                    text: "H.264",
                },
                {
                    key: "h265",
                    text: "H.265",
                },
            ],
        },
        {
            group: "settings",
            key: "videoEncoder",
            type: "dropdown",
            label: "视频编编码器",
            placeholder:
                SETTING_STATE.encoders.length === 0
                    ? "按刷新按钮更新编码器列表"
                    : "(默认)",
            labelExtra: (
                <IconButton
                    iconProps={{ iconName: Icons.ArrowClockwise }}
                    disabled={!GLOBAL_STATE.adb}
                    text="刷新"
                    onClick={async () => {
                        try {
                            await STATE.pushServer();

                            const encoders = await AdbScrcpyClient.getEncoders(
                                GLOBAL_STATE.adb!,
                                DEFAULT_SERVER_PATH,
                                VERSION,
                                new AdbScrcpyOptionsLatest(
                                    new ScrcpyOptionsLatest({
                                        logLevel: ScrcpyLogLevel.Debug,
                                    })
                                )
                            );

                            runInAction(() => {
                                SETTING_STATE.encoders = encoders;
                            });
                        } catch (e: any) {
                            GLOBAL_STATE.showErrorDialog(e);
                        }
                    }}
                />
            ),
            options: SETTING_STATE.encoders
                .filter(
                    (item) =>
                        item.type === "video" &&
                        (!item.codec ||
                            item.codec === SETTING_STATE.settings.videoCodec!)
                )
                .map((item) => ({
                    key: item.name,
                    text: item.name,
                })),
        }
    );

    result.push({
        group: "settings",
        key: "lockVideoOrientation",
        type: "dropdown",
        label: "锁定视频方向",
        options: [
            {
                key: ScrcpyVideoOrientation.Unlocked,
                text: "Unlocked",
            },
            {
                key: ScrcpyVideoOrientation.Initial,
                text: "Current",
            },
            {
                key: ScrcpyVideoOrientation.Portrait,
                text: "Portrait",
            },
            {
                key: ScrcpyVideoOrientation.Landscape,
                text: "Landscape",
            },
            {
                key: ScrcpyVideoOrientation.PortraitFlipped,
                text: "Portrait (Flipped)",
            },
            {
                key: ScrcpyVideoOrientation.LandscapeFlipped,
                text: "Landscape (Flipped)",
            },
        ],
    });

    if (SETTING_STATE.decoders.length > 1) {
        result.push({
            group: "clientSettings",
            key: "decoder",
            type: "dropdown",
            label: "视频解码器",
            options: SETTING_STATE.decoders.map((item) => ({
                key: item.key,
                text: item.name,
                data: item,
            })),
        });
    }

    result.push({
        group: "clientSettings",
        key: "ignoreDecoderCodecArgs",
        type: "toggle",
        label: `忽略视频解码器的编解码器选项`,
        description: `有些解码器不支持所有 H.264 配置文件/级别，因此它们要求设备以它们支持的最高编解码器进行编码。但是，有些超旧设备可能不支持该编解码器，因此它们的编码器将无法启动。使用此选项可让设备选择要使用的编解码器。`,
    });

    result.push(
        {
            group: "settings",
            key: "audio",
            type: "toggle",
            label: "转发音频（需要 Android 11）",
        },
        {
            group: "settings",
            key: "audioCodec",
            type: "dropdown",
            label: "音频编解码器",
            options: [
                {
                    key: "raw",
                    text: "Raw",
                },
                {
                    key: "aac",
                    text: "AAC",
                },
                {
                    key: "opus",
                    text: "Opus",
                },
            ],
        },
        {
            group: "settings",
            key: "audioEncoder",
            type: "dropdown",
            placeholder:
                SETTING_STATE.encoders.length === 0
                    ? "按刷新按钮更新编码器列表"
                    : "(默认)",
            label: "音频编码器",
            labelExtra: (
                <IconButton
                    iconProps={{ iconName: Icons.ArrowClockwise }}
                    disabled={!GLOBAL_STATE.adb}
                    text="刷新"
                    onClick={async () => {
                        try {
                            await STATE.pushServer();

                            const encoders = await AdbScrcpyClient.getEncoders(
                                GLOBAL_STATE.adb!,
                                DEFAULT_SERVER_PATH,
                                VERSION,
                                new AdbScrcpyOptionsLatest(
                                    new ScrcpyOptionsLatest({
                                        logLevel: ScrcpyLogLevel.Debug,
                                    })
                                )
                            );

                            runInAction(() => {
                                SETTING_STATE.encoders = encoders;
                            });
                        } catch (e: any) {
                            GLOBAL_STATE.showErrorDialog(e);
                        }
                    }}
                />
            ),
            options: SETTING_STATE.encoders
                .filter(
                    (x) =>
                        x.type === "audio" &&
                        x.codec === SETTING_STATE.settings.audioCodec
                )
                .map((item) => ({
                    key: item.name,
                    text: item.name,
                })),
        }
    );

    return result;
});

autorun(() => {
    if (SETTING_STATE.encoders.length === 0) {
        SETTING_STATE.settings.videoEncoder = "";
        SETTING_STATE.settings.audioEncoder = "";
        return;
    }

    const encodersForCurrentVideoCodec = SETTING_STATE.encoders.filter(
        (item) =>
            item.type === "video" &&
            item.codec === SETTING_STATE.settings.videoCodec
    );
    if (
        SETTING_STATE.settings.videoEncoder &&
        encodersForCurrentVideoCodec.every(
            (item) => item.name !== SETTING_STATE.settings.videoEncoder
        )
    ) {
        SETTING_STATE.settings.videoEncoder = "";
    }

    const encodersForCurrentAudioCodec = SETTING_STATE.encoders.filter(
        (item) =>
            item.type === "audio" &&
            item.codec === SETTING_STATE.settings.audioCodec
    );
    if (
        SETTING_STATE.settings.audioEncoder &&
        encodersForCurrentAudioCodec.every(
            (item) => item.name !== SETTING_STATE.settings.audioEncoder
        )
    ) {
        SETTING_STATE.settings.audioEncoder = "";
    }
});
