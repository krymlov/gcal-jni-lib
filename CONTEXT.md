<!-- generated-by: ai-github-projects/tools/aar_context.py -->

# CONTEXT.md — `gcal-jni-lib`

> База знань про цю AAR-бібліотеку: що вона дає застосунку і як нею
> користуватись. Розділи, позначені **GENERATED**, пише
> `ai-github-projects/tools/aar_context.py` з реальних файлів — правити їх
> руками немає сенсу, вони перезапишуться. Проза живе в
> `tools/aar_context_prose.py`.

## Коротко

**Нативна бібліотека календаря Ґаурабда** — `.so` під чотири ABI і нічого більше:
ні Java, ні ресурсів. Java-обгортка живе в `gaurabda-suppart` (`GCalManager`).

## Підключити

```gradle
implementation 'org.gaurabda:gcal-android:11j3'
```

## Що дає застосунку

`GCalManager.loadLibrary()` у `gaurabda-suppart`, далі `nativeGCal4Query(...)`.

## Залежності — GENERATED

Жодних — самодостатня.

## Вміст — GENERATED

| що | скільки |
|---|---|
| `jniLibs/arm64-v8a` | libgcal-11j.so |
| `jniLibs/armeabi-v7a` | libgcal-11j.so |
| `jniLibs/x86` | libgcal-11j.so |
| `jniLibs/x86_64` | libgcal-11j.so |

## Java-поверхня — GENERATED

_Java-коду немає._ Це чисто ресурсна/асетна бібліотека.

## Пастки

**JNI-точки входу резолвляться за іменем символу**, тож ні клас, ні нативні
методи не можна перейменовувати. `consumer-rules.pro` тут тримає
`-keep class org.gaurabda.GCalManager { native <methods>; }` — саме тому, що застосунки
збираються з `minifyEnabled true`.

## Координати й оточення — GENERATED

|  |  |
|---|---|
| Maven | `org.gaurabda:gcal-android:11j3` |
| namespace | `org.gaurabda` |
| SDK | compile 37, min 21, target 36 |
| AndroidManifest | порожній `<manifest/>` |
| дозволи | жодного |
| хто вже використовує | ніхто |

## Збірка

Однакова для всіх 24 AAR-бібліотек — еталон `holidates-suppart`, уся метадата в
`gradle.properties` (`AAR_*`). Перезібрати й опублікувати **все** в правильному порядку:

```bash
cd /e/Softworks/Holidates/ai-github-projects/tools
"$PYTHON_HOME/python.exe" aar_publish.py --all
```

Порядок рахується з `dependencies`, бо через `mavenLocal()` публікація в неправильному порядку
дає збірку, що мовчки підтягла несвіжу залежність. Подробиці —
`ai-github-projects/tools/README-aar.md`.
