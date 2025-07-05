# cJSON

ANSI C 超轻量级 JSON 解析器(parser)。

## 目录
* [许可证](#license)
* [用法](#usage)
  * [欢迎使用 cJSON](#welcome-to-cjson)
  * [构建](#building)
    * [复制源码](#copying-the-source)
    * [CMake](#cmake)
    * [Makefile](#makefile)
    * [Meson](#meson)
    * [Vcpkg](#Vcpkg)
  * [包含 cJSON](#including-cjson)
  * [数据结构](#data-structure)
  * [操作数据结构](#working-with-the-data-structure)
    * [基本类型](#basic-types)
    * [数组](#arrays)
    * [对象](#objects)
  * [解析 JSON](#parsing-json)
  * [打印 JSON](#printing-json)
  * [示例](#example)
    * [打印](#printing)
    * [解析](#parsing)
  * [注意事项](#caveats)
    * [零字符](#zero-character)
    * [字符编码](#character-encoding)
    * [C 标准](#c-standard)
    * [浮点数](#floating-point-numbers)
    * [数组和对象的深层嵌套](#deep-nesting-of-arrays-and-objects)
    * [线程安全](#thread-safety)
    * [大小写敏感性](#case-sensitivity)
    * [重复的对象成员](#duplicate-object-members)
  * [享受 cJSON!](#enjoy-cjson)

## 许可证

MIT 许可证

>  版权所有 (c) 2009-2017 Dave Gamble 及 cJSON 贡献者
>
>  特此免费授予任何获得本软件及相关文档文件（"软件"）副本的人不受限制地处理本软件的权限，包括但不限于使用、复制、修改、合并、发布、分发、再许可和/或销售本软件副本的权利，并允许向其提供本软件的人这样做，但须符合以下条件：
>
>  上述版权声明和本许可声明应包含在本软件的所有副本或主要部分中。
>
>  本软件按"原样"提供，不提供任何形式的明示或暗示保证，包括但不限于适销性、特定用途适用性和非侵权保证。在任何情况下，作者或版权持有人均不对任何索赔、损害或其他责任负责，无论是在合同诉讼、侵权行为还是其他方面，由本软件或本软件的使用或其他交易引起、由本软件引起或与之相关。

## 用法

### 欢迎使用 cJSON

cJSON 旨在成为能够完成工作的最简单解析器(parser)。它是单个 C 源文件和一个头文件。

JSON 的最佳描述见：http://www.json.org/
它类似于 XML，但更加精简。您可以用它来传输数据、存储内容，或表示程序状态。

作为一个库，cJSON 旨在尽可能减少繁琐工作，同时不阻碍您的开发。从实用角度出发（即忽略绝对真理），您可以在两种模式下使用它：自动模式和手动模式。让我们快速了解一下。

我从这个页面提取了一些 JSON：http://www.json.org/fatfree.html
该页面启发我编写了 cJSON，这是一个尝试与 JSON 本身共享相同理念的解析器：简单、精简、不碍事。

### 构建

有几种方法可将 cJSON 集成到您的项目中。

#### 复制源码

由于整个库仅包含一个 C 文件和一个头文件，您只需将 `cJSON.h` 和 `cJSON.c` 复制到项目源码中即可开始使用。

cJSON 使用 ANSI C (C89) 编写，以支持尽可能多的平台和编译器。

#### CMake

使用 CMake 时，cJSON 支持完整的构建系统。这种方式可获得最多的功能。支持 CMake 2.8.5 或更高版本。推荐使用树外编译(out of tree build)，即编译文件放在与源文件分离的目录中。因此，要在 Unix 平台上使用 CMake 构建 cJSON，请创建 `build` 目录并在其中运行 CMake：

```
mkdir build
cd build
cmake ..
```




这将创建 Makefile 和其他文件。然后可以编译：

```
make
```




如果需要，可以使用 `make install` 安装。默认情况下，头文件安装在 `/usr/local/include/cjson`，库文件安装在 `/usr/local/lib`。同时安装 pkg-config 文件以便更容易检测和使用已安装的 CMake。还安装 CMake 配置文件，供其他基于 CMake 的项目用于发现该库。

您可以通过向 CMake 传递不同的选项来更改构建过程。使用 `On` 开启选项，`Off` 关闭选项：

* `-DENABLE_CJSON_TEST=On`: 启用构建测试。（默认开启）
* `-DENABLE_CJSON_UTILS=On`: 启用构建 cJSON_Utils。（默认关闭）
* `-DENABLE_TARGET_EXPORT=On`: 启用 CMake 目标的导出。如果出现问题请关闭。（默认开启）
* `-DENABLE_CUSTOM_COMPILER_FLAGS=On`: 启用自定义编译器标志（当前支持 Clang、GCC 和 MSVC）。如果出现问题请关闭。（默认开启）
* `-DENABLE_VALGRIND=On`: 使用 [valgrind](http://valgrind.org) 运行测试。（默认关闭）
* `-DENABLE_SANITIZERS=On`: 使用 [AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer) 和 [UndefinedBehaviorSanitizer](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html) 编译 cJSON（如果可能）。（默认关闭）
* `-DENABLE_SAFE_STACK`: 启用 [SafeStack](https://clang.llvm.org/docs/SafeStack.html) 检测通道。目前仅适用于 Clang 编译器。（默认关闭）
* `-DBUILD_SHARED_LIBS=On`: 构建共享库。（默认开启）
* `-DBUILD_SHARED_AND_STATIC_LIBS=On`: 同时构建共享库和静态库。（默认关闭）
* `-DCMAKE_INSTALL_PREFIX=/usr`: 设置安装前缀。
* `-DENABLE_LOCALES=On`: 启用 localeconv 方法的使用。（默认开启）
* `-DCJSON_OVERRIDE_BUILD_SHARED_LIBS=On`: 允许使用 `-DCJSON_BUILD_SHARED_LIBS` 覆盖 `BUILD_SHARED_LIBS` 的值。
* `-DENABLE_CJSON_VERSION_SO`: 启用 cJSON so 版本。（默认开启）

如果您要为 Linux 发行版打包 cJSON，可以执行以下步骤：

```
mkdir build
cd build
cmake .. -DENABLE_CJSON_UTILS=On -DENABLE_CJSON_TEST=Off -DCMAKE_INSTALL_PREFIX=/usr
make
make DESTDIR=$pkgdir install
```




在 Windows 上，通常通过在 Visual Studio 的开发人员命令提示符中运行 CMake 来创建 Visual Studio 解决方案文件。具体步骤请遵循 CMake 和 Microsoft 的官方文档，并使用您选择的搜索引擎。上述选项的描述通常仍然适用，尽管并非所有选项都在 Windows 上有效。

#### Makefile

**注意：** 此方法已弃用。尽可能使用 CMake。Makefile 支持仅限于错误修复。

如果没有 CMake，但仍有 GNU make，可以使用 Makefile 构建 cJSON：

在源码目录中运行此命令，它将自动编译静态库、共享库和一个小测试程序（非完整测试套件）。

make all


如果需要，可以使用 `make install` 将编译后的库安装到系统中。默认情况下，头文件安装在 `/usr/local/include/cjson`，库文件安装在 `/usr/local/lib`。但您可以通过设置 `PREFIX` 和 `DESTDIR` 变量来更改此行为：`make PREFIX=/usr DESTDIR=temp install`。使用以下命令卸载：`make PREFIX=/usr DESTDIR=temp uninstall`。

#### Meson

要在使用 meson 的项目中使用 cjson，需要包含 libcjson 依赖：

```meson
project('c-json-example', 'c')

cjson = dependency('libcjson')

example = executable(
    'example',
    'example.c',
    dependencies: [cjson],
)
```

#### Vcpkg

您可以使用 [vcpkg](https://github.com/Microsoft/vcpkg) 依赖管理器下载并安装 cJSON：
```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
vcpkg install cjson
```

vcpkg 中的 cJSON 端口由 Microsoft 团队成员和社区贡献者保持更新。如果版本过时，请在 vcpkg 仓库中[创建问题或提交拉取请求](https://github.com/Microsoft/vcpkg)。

### 包含 cJSON

如果通过 CMake 或 Makefile 安装，可以这样包含 cJSON：

```c
#include <cjson/cJSON.h>
```

### 数据结构

cJSON 使用 `cJSON` 结构体表示 JSON 数据：

```c
/* cJSON 结构体： */
typedef struct cJSON
{
    struct cJSON *next;
    struct cJSON *prev;
    struct cJSON *child;
    int type;
    char *valuestring;
    /* 写入 valueint 已弃用，请改用 cJSON_SetNumberValue */
    int valueint;
    double valuedouble;
    char *string;
} cJSON;
```

此类型的项目表示一个 JSON 值。类型以位标志(bit-flag)形式存储在 `type` 中（**这意味着您不能仅通过比较 `type` 的值来判断类型**）。

要检查项目的类型，请使用相应的 `cJSON_Is...` 函数。它执行 `NULL` 检查后再进行类型检查，如果项目属于该类型则返回布尔值。

类型可以是以下之一：

* `cJSON_Invalid` (用 `cJSON_IsInvalid` 检查)：表示不包含任何值的无效项目。如果将项目设置为全零字节，则自动具有此类型。
* `cJSON_False` (用 `cJSON_IsFalse` 检查)：表示 `false` 布尔值。也可以使用 `cJSON_IsBool` 检查一般的布尔值。
* `cJSON_True` (用 `cJSON_IsTrue` 检查)：表示 `true` 布尔值。也可以使用 `cJSON_IsBool` 检查一般的布尔值。
* `cJSON_NULL` (用 `cJSON_IsNull` 检查)：表示 `null` 值。
* `cJSON_Number` (用 `cJSON_IsNumber` 检查)：表示数值。值以 double 形式存储在 `valuedouble` 中，同时存储在 `valueint` 中。如果数字超出整数范围，则 `valueint` 使用 `INT_MAX` 或 `INT_MIN`。
* `cJSON_String` (用 `cJSON_IsString` 检查)：表示字符串值。它以零终止字符串的形式存储在 `valuestring` 中。
* `cJSON_Array` (用 `cJSON_IsArray` 检查)：表示数组值。通过将 `child` 指向表示数组值的 `cJSON` 项目的链表来实现。元素使用 `next` 和 `prev` 链接在一起，其中第一个元素的 `prev.next == NULL`，最后一个元素的 `next == NULL`。
* `cJSON_Object` (用 `cJSON_IsObject` 检查)：表示对象值。对象的存储方式与数组相同，唯一区别是对象中的项目将其键存储在 `string` 中。
* `cJSON_Raw` (用 `cJSON_IsRaw` 检查)：表示任何类型的 JSON，以零终止字符数组的形式存储在 `valuestring` 中。例如，可用于避免重复打印相同的静态 JSON 以节省性能。cJSON 在解析时永远不会创建此类型。另请注意，cJSON 不检查它是否为有效的 JSON。

此外，还有以下两个标志：

* `cJSON_IsReference`：指示 `child` 指向的项目和/或 `valuestring` 不属于此项目，它只是一个引用。因此 `cJSON_Delete` 和其他函数只会释放此项目，而不会释放其 `child`/`valuestring`。
* `cJSON_StringIsConst`：表示 `string` 指向常量字符串。这意味着 `cJSON_Delete` 和其他函数不会尝试释放 `string`。

### 操作数据结构

对于每种值类型，都有一个 `cJSON_Create...` 函数可用于创建该类型的项目。
所有这些函数都会分配一个 `cJSON` 结构体，稍后可以使用 `cJSON_Delete` 删除。
请注意，您必须在某个时候删除它们，否则会导致内存泄漏。  
**重要提示**：如果已将项目添加到数组或对象中，则**不得**使用 `cJSON_Delete` 删除它。将其添加到数组或对象会转移其所有权，因此当该数组或对象被删除时，它也会被删除。您也可以使用 `cJSON_SetValuestring` 更改 `cJSON_String` 的 `valuestring`，并且无需手动释放先前的 `valuestring`。

#### 基本类型

* **null** 使用 `cJSON_CreateNull` 创建
* **布尔值** 使用 `cJSON_CreateTrue`、`cJSON_CreateFalse` 或 `cJSON_CreateBool` 创建
* **数字** 使用 `cJSON_CreateNumber` 创建。这将同时设置 `valuedouble` 和 `valueint`。如果数字超出整数范围，则 `valueint` 使用 `INT_MAX` 或 `INT_MIN`
* **字符串** 使用 `cJSON_CreateString` 创建（复制字符串）或使用 `cJSON_CreateStringReference` 创建（直接指向字符串。这意味着 `valuestring` 不会被 `cJSON_Delete` 删除，您需要负责其生命周期，适用于常量）

#### 数组

您可以使用 `cJSON_CreateArray` 创建一个空数组。`cJSON_CreateArrayReference` 可用于创建一个不"拥有"其内容的数组，因此其内容不会被 `cJSON_Delete` 删除。

要将项目添加到数组，请使用 `cJSON_AddItemToArray` 将项目追加到末尾。
使用 `cJSON_AddItemReferenceToArray` 可以将元素添加为对其他项目、数组或字符串的引用。这意味着 `cJSON_Delete` 不会删除该项目的 `child` 或 `valuestring` 属性，因此如果它们已在其他地方使用，则不会发生双重释放。
要在中间插入项目，请使用 `cJSON_InsertItemInArray`。它将在给定的从 0 开始的索引处插入一个项目，并将所有现有项目向右移动。

如果要从数组中取出给定索引处的项目并继续使用它，请使用 `cJSON_DetachItemFromArray`，它将返回分离的项目，因此请确保将其分配给指针，否则会导致内存泄漏。

使用 `cJSON_DeleteItemFromArray` 删除项目。它的工作方式类似于 `cJSON_DetachItemFromArray`，但通过 `cJSON_Delete` 删除分离的项目。

您还可以就地替换数组中的项目。使用 `cJSON_ReplaceItemInArray`（通过索引）或 `cJSON_ReplaceItemViaPointer`（给定指向元素的指针）。如果失败，`cJSON_ReplaceItemViaPointer` 将返回 `0`。其内部操作是分离旧项目，删除它，并在其位置插入新项目。

要获取数组的大小，请使用 `cJSON_GetArraySize`。使用 `cJSON_GetArrayItem` 获取给定索引处的元素。

由于数组存储为链表，通过索引迭代效率低下（`O(n²)`），因此您可以使用 `cJSON_ArrayForEach` 宏在 `O(n)` 时间复杂度内迭代数组。

#### 对象

您可以使用 `cJSON_CreateObject` 创建一个空对象。`cJSON_CreateObjectReference` 可用于创建一个不"拥有"其内容的对象，因此其内容不会被 `cJSON_Delete` 删除。

要将项目添加到对象，请使用 `cJSON_AddItemToObject`。使用 `cJSON_AddItemToObjectCS` 将项目添加到对象中，其名称是常量或引用（项目的键，即 `cJSON` 结构体中的 `string`），因此它不会被 `cJSON_Delete` 释放。
使用 `cJSON_AddItemReferenceToObject` 可以将元素添加为对其他对象、数组或字符串的引用。这意味着 `cJSON_Delete` 不会删除该项目的 `child` 或 `valuestring` 属性，因此如果它们已在其他地方使用，则不会发生双重释放。

如果要从对象中取出项目，请使用 `cJSON_DetachItemFromObjectCaseSensitive`，它将返回分离的项目，因此请确保将其分配给指针，否则会导致内存泄漏。

使用 `cJSON_DeleteItemFromObjectCaseSensitive` 删除项目。它的工作方式类似于 `cJSON_DetachItemFromObjectCaseSensitive` 后跟 `cJSON_Delete`。

您还可以就地替换对象中的项目。使用 `cJSON_ReplaceItemInObjectCaseSensitive`（通过键）或 `cJSON_ReplaceItemViaPointer`（给定指向元素的指针）。如果失败，`cJSON_ReplaceItemViaPointer` 将返回 `0`。其内部操作是分离旧项目，删除它，并在其位置插入新项目。

要获取对象的大小，可以使用 `cJSON_GetArraySize`，这之所以有效是因为对象在内部存储为数组。

如果要访问对象中的项目，请使用 `cJSON_GetObjectItemCaseSensitive`。

要迭代对象，可以像处理数组一样使用 `cJSON_ArrayForEach` 宏。

cJSON 还提供了便捷的帮助函数，用于快速创建新项目并将其添加到对象中，例如 `cJSON_AddNullToObject`。它们返回指向新项目的指针，如果失败则返回 `NULL`。

### 解析 JSON

给定一个零终止字符串中的 JSON，您可以使用 `cJSON_Parse` 解析它。

```c
cJSON *json = cJSON_Parse(string);
```

给定字符串中的 JSON（无论是否零终止），您可以使用 `cJSON_ParseWithLength` 解析它。

```c
cJSON *json = cJSON_ParseWithLength(string, buffer_length);
```

它将解析 JSON 并分配一个表示它的 `cJSON` 项目树。返回后，您完全负责在使用后通过 `cJSON_Delete` 释放它。

`cJSON_Parse` 使用的分配器默认为 `malloc` 和 `free`，但可以使用 `cJSON_InitHooks` 更改（全局）。

如果发生错误，可以使用 `cJSON_GetErrorPtr` 访问输入字符串中错误位置的指针。但请注意，这在多线程场景中可能产生竞争条件，在这种情况下，最好使用带有 `return_parse_end` 的 `cJSON_ParseWithOpts`。
默认情况下，输入字符串中已解析 JSON 之后的字符不会被视为错误。

如果需要更多选项，请使用 `cJSON_ParseWithOpts(const char *value, const char **return_parse_end, cJSON_bool require_null_terminated)`。
`return_parse_end` 返回指向输入字符串中 JSON 结尾的指针或发生错误的位置（从而以线程安全的方式替代 `cJSON_GetErrorPtr`）。如果 `require_null_terminated` 设置为 `1`，则输入字符串在 JSON 之后包含数据将被视为错误。

如果需要更多提供缓冲区长度的选项，请使用 `cJSON_ParseWithLengthOpts(const char *value, size_t buffer_length, const char **return_parse_end, cJSON_bool require_null_terminated)`。

### 打印 JSON

给定一个 `cJSON` 项目树，您可以使用 `cJSON_Print` 将它们打印为字符串。

```c
char *string = cJSON_Print(json);
```

它将分配一个字符串并将树的 JSON 表示形式打印到其中。返回后，您完全负责在使用后通过您的分配器释放它（通常是 `free`，具体取决于通过 `cJSON_InitHooks` 设置的内容）。

`cJSON_Print` 将使用空白符进行格式化打印。如果不需要格式化，请使用 `cJSON_PrintUnformatted`。

如果大致了解结果字符串的大小，可以使用 `cJSON_PrintBuffered(const cJSON *item, int prebuffer, cJSON_bool fmt)`。`fmt` 是一个布尔值，用于开启或关闭带空白符的格式化。`prebuffer` 指定用于打印的初始缓冲区大小。`cJSON_Print` 当前使用 256 字节作为其初始缓冲区大小。当打印空间不足时，会分配一个新缓冲区，并在继续打印之前复制旧缓冲区的内容。

通过使用 `cJSON_PrintPreallocated(cJSON *item, char *buffer, const int length, const cJSON_bool format)` 可以完全避免这些动态缓冲区分配。它接受一个指向要打印到的缓冲区的指针及其长度。如果达到长度，打印将失败并返回 `0`。如果成功，则返回 `1`。请注意，您应提供比实际需要多 5 字节的内存，因为 cJSON 在估计提供的内存是否足够时并非 100% 准确。

### 示例

在此示例中，我们要构建并解析以下 JSON：

```json
{
    "name": "Awesome 4K",
    "resolutions": [
        {
            "width": 1280,
            "height": 720
        },
        {
            "width": 1920,
            "height": 1080
        },
        {
            "width": 3840,
            "height": 2160
        }
    ]
}
```

#### 打印

让我们构建上述 JSON 并将其打印为字符串：

```c
// 创建一个包含支持分辨率列表的显示器
// 注意：返回堆分配的字符串，使用后需要释放
char *create_monitor(void)
{
    const unsigned int resolution_numbers[3][2] = {
        {1280, 720},
        {1920, 1080},
        {3840, 2160}
    };
    char *string = NULL;
    cJSON *name = NULL;
    cJSON *resolutions = NULL;
    cJSON *resolution = NULL;
    cJSON *width = NULL;
    cJSON *height = NULL;
    size_t index = 0;

    cJSON *monitor = cJSON_CreateObject();
    if (monitor == NULL)
    {
        goto end;
    }

    name = cJSON_CreateString("Awesome 4K");
    if (name == NULL)
    {
        goto end;
    }
    /* 创建成功后，立即将其添加到显示器，
     * 从而将指针的所有权转移给它 */
    cJSON_AddItemToObject(monitor, "name", name);

    resolutions = cJSON_CreateArray();
    if (resolutions == NULL)
    {
        goto end;
    }
    cJSON_AddItemToObject(monitor, "resolutions", resolutions);

    for (index = 0; index < (sizeof(resolution_numbers) / (2 * sizeof(int))); ++index)
    {
        resolution = cJSON_CreateObject();
        if (resolution == NULL)
        {
            goto end;
        }
        cJSON_AddItemToArray(resolutions, resolution);

        width = cJSON_CreateNumber(resolution_numbers[index][0]);
        if (width == NULL)
        {
            goto end;
        }
        cJSON_AddItemToObject(resolution, "width", width);

        height = cJSON_CreateNumber(resolution_numbers[index][1]);
        if (height == NULL)
        {
            goto end;
        }
        cJSON_AddItemToObject(resolution, "height", height);
    }

    string = cJSON_Print(monitor);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print monitor.\n");
    }

end:
    cJSON_Delete(monitor);
    return string;
}
```

或者，我们可以使用 `cJSON_Add...ToObject` 帮助函数让生活更轻松：

```c
// 注意：返回堆分配的字符串，使用后需要释放
char *create_monitor_with_helpers(void)
{
    const unsigned int resolution_numbers[3][2] = {
        {1280, 720},
        {1920, 1080},
        {3840, 2160}
    };
    char *string = NULL;
    cJSON *resolutions = NULL;
    size_t index = 0;

    cJSON *monitor = cJSON_CreateObject();

    if (cJSON_AddStringToObject(monitor, "name", "Awesome 4K") == NULL)
    {
        goto end;
    }

    resolutions = cJSON_AddArrayToObject(monitor, "resolutions");
    if (resolutions == NULL)
    {
        goto end;
    }

    for (index = 0; index < (sizeof(resolution_numbers) / (2 * sizeof(int))); ++index)
    {
        cJSON *resolution = cJSON_CreateObject();

        if (cJSON_AddNumberToObject(resolution, "width", resolution_numbers[index][0]) == NULL)
        {
            goto end;
        }

        if (cJSON_AddNumberToObject(resolution, "height", resolution_numbers[index][1]) == NULL)
        {
            goto end;
        }

        cJSON_AddItemToArray(resolutions, resolution);
    }

    string = cJSON_Print(monitor);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print monitor.\n");
    }

end:
    cJSON_Delete(monitor);
    return string;
}
```

#### 解析

在此示例中，我们将解析上述格式的 JSON，并在打印一些诊断输出的同时检查显示器是否支持全高清分辨率：

```c
/* 如果显示器支持全高清则返回 1，否则返回 0 */
int supports_full_hd(const char * const monitor)
{
    const cJSON *resolution = NULL;
    const cJSON *resolutions = NULL;
    const cJSON *name = NULL;
    int status = 0;
    cJSON *monitor_json = cJSON_Parse(monitor);
    if (monitor_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        status = 0;
        goto end;
    }

    name = cJSON_GetObjectItemCaseSensitive(monitor_json, "name");
    if (cJSON_IsString(name) && (name->valuestring != NULL))
    {
        printf("Checking monitor \"%s\"\n", name->valuestring);
    }

    resolutions = cJSON_GetObjectItemCaseSensitive(monitor_json, "resolutions");
    cJSON_ArrayForEach(resolution, resolutions)
    {
        cJSON *width = cJSON_GetObjectItemCaseSensitive(resolution, "width");
        cJSON *height = cJSON_GetObjectItemCaseSensitive(resolution, "height");

        if (!cJSON_IsNumber(width) || !cJSON_IsNumber(height))
        {
            status = 0;
            goto end;
        }

        if ((width->valuedouble == 1920) && (height->valuedouble == 1080))
        {
            status = 1;
            goto end;
        }
    }

end:
    cJSON_Delete(monitor_json);
    return status;
}
```

请注意，除了 `cJSON_Parse` 的结果外，没有 `NULL` 检查，因为 `cJSON_GetObjectItemCaseSensitive` 已经检查了 `NULL` 输入，因此 `NULL` 值只会传播，并且如果输入为 `NULL`，`cJSON_IsNumber` 和 `cJSON_IsString` 会返回 `0`。

### 注意事项

#### 零字符

cJSON 不支持包含零字符 `'\0'` 或 `\u0000` 的字符串。由于字符串是零终止的，这在当前 API 中是不可能的。

#### 字符编码

cJSON 仅支持 UTF-8 编码的输入。在大多数情况下，它不会拒绝无效的 UTF-8 输入，而是按原样传播。只要输入不包含无效的 UTF-8，输出将始终是有效的 UTF-8。

#### C 标准

cJSON 使用 ANSI C（或 C89、C90）编写。如果您的编译器或 C 库不遵循此标准，则无法保证行为正确。

注意：ANSI C 不是 C++，因此不应使用 C++ 编译器编译。但是，您可以使用 C 编译器编译它，并将其与 C++ 代码链接。尽管使用 C++ 编译器编译可能有效，但不保证行为正确。

#### 浮点数

cJSON 官方仅支持 IEEE754 双精度浮点数(double precision floating point numbers)。它可能仍适用于其他实现，但这些实现中的错误将被视为无效。

cJSON 当前支持的浮点数字面量的最大长度为 63 个字符。

#### 数组和对象的深层嵌套

cJSON 不支持嵌套过深的数组和对象，因为这会导致堆栈溢出。为防止这种情况，cJSON 将深度限制为 `CJSON_NESTING_LIMIT`，默认为 1000，但可以在编译时更改。

#### 线程安全

通常 cJSON **不是线程安全**的。

但在以下条件下是线程安全的：

* 从不使用 `cJSON_GetErrorPtr`（可以使用 `cJSON_ParseWithOpts` 的 `return_parse_end` 参数替代）
* `cJSON_InitHooks` 仅在所有线程使用 cJSON 之前调用。
* 在所有对 cJSON 函数的调用返回之前，从不调用 `setlocale`。

#### 大小写敏感性

最初创建 cJSON 时，它不遵循 JSON 标准，并且不区分大小写字母。如果需要正确、符合标准的行为，需要在可用时使用 `CaseSensitive` 函数。

#### 重复的对象成员

cJSON 支持解析和打印包含具有同名多个成员的对象。但是 `cJSON_GetObjectItemCaseSensitive` 将始终只返回第一个。

# 享受 cJSON!

- Dave Gamble (原始作者)
- Max Bruckner 和 Alan Wang (当前维护者)
- 以及其他 [cJSON 贡献者](CONTRIBUTORS.md)

```