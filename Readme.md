# regmklink

A specialized Windows registry symbolic link creation tool for security research and advanced system configuration.

## Overview

`regmklink` is a command-line utility designed to create and delete symbolic links within the Windows Registry.

I developed this tool while conducting related research and experimenting with registry after discovering that Pavel Yosifovich's [regeditx.exe](https://github.com/zodiacon/RegEditX) tool, which he mentioned in his [Creating Registry Links](https://scorpiosoftware.net/2020/07/17/creating-registry-links/) article, had been removed from his [AllTools](https://github.com/zodiacon/AllTools/tree/master) repository.

This tool is very basic. Feel free to suggest or changes or improve it if you have ideas (-;

## Usage

```
regmklink.exe <Hive> <Link> [<Target>|-d]
```

- `<Hive>`: The registry hive (e.g., HKLM).
- `<Link>`: The native registry path of the symbolic link to create or delete.
- `<Target>`: The native registry path of the target key.
- `-d`: Deletes the specified symbolic link.

**Note:** `<Link>` requires the native registry path. To inspect the native path to your key, you can use the [TotalReg](https://github.com/zodiacon/AllTools/blob/master/TotalReg.exe) tool by Pavel Yosifovich.

## Examples

### Create a Symbolic Link

```
regmklink.exe HKLM "\REGISTRY\MACHINE\SOFTWARE\MySymLink" "\REGISTRY\MACHINE\SOFTWARE\TargetKey"
```

### Delete a Symbolic Link

```
regmklink.exe HKLM "\REGISTRY\MACHINE\SOFTWARE\MySymLink" -d
```

## Short Technical Summary

Registry symbolic links are keys that point to other keys within the same hive. The native windows registry editor (`regedit.exe`) actually does not show any visible distinction between regular keys and link keys. However, a common example for such symlink is - `HKEY_LOCAL_MACHINE\System\CurrentControlSet`, which links to `HKEY_LOCAL_MACHINE\System\ControlSet001` (usually).

### Creating a Registry Link

1. When created a key using `RegCreateKeyEx`, this flag should be used: `REG_OPTION_CREATE_LINK` - specifying that the key is intended to function as a link rather than a standard key.
2. Under the new "link" key, what actually points to the target key is a string value named `SymbolicLinkValue`. (\*path to the target key must be mentioned as native/absolute).

### Deleting a Registry Link

Note that deleting link keys is a bit more complicated than creating ones, since this requires using the native `NtDeleteKey` function, which has to be dynamically resolved from `ntdll.dll`,



** For more details, refer to Pavel Yosifovich's article linked above.

