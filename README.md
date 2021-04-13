# MIDL

> Mini Interactive Description language。
>
> 实现MIDL的编译器前端。

## 词法分析

* 16个关键字，不区分大小写

  | `struct` | `float`  | `boolean` | `short`    |
  | -------- | -------- | --------- | ---------- |
  | `long`   | `double` | `int8`    | `int16`    |
  | `int32`  | `int64`  | `uint8`   | `uint16`   |
  | `uint32` | `uint64` | `char`    | `unsigned` |

* 17个专用符号

  | `{`  | `}`  | `;`  | `[`  | `]`  |
  | :--: | :--: | :--: | :--: | :--: |
  | `*`  | `+`  | `-`  | `~`  | `/`  |
  | `%`  | `>>` | `<<` | `&`  | `^`  |
  | `|`  | `,`  |      |      |      |

* 4种类型

  * `ID = LETTER (UNDERLINE?(LETTER | DIGIT))*` 其中`LETTER = [a-z] | [A- Z]`、`DIGIT = [0-9]`、`UNDERLINE= _`
  * `INTEGER  = (0 | [1-9] [0-9]*) INTEGER_TYPE_SUFFIX?` 其中`INTEGER_TYPE_SUFFIX =  l | L`
  * `STRING = "(ESCAPE_SEQUENCE | (~\ | ~"))*" ` 其中 `ESCAPE_SEQUENCE =  \ (b | t | n | f | r | " | \ )`
  * `BOOLEAN = TRUE | FALSE`

  > *表示闭包，?表示0/1个，`~\`表示\除外的字符，()表示优先级

* DFA分析

  <img src="res/DFA.png" style="zoom: 25%;" />

  * 上述DFA将16个关键字和BOOlEAN类型识别为ID，这不是BUG，而是为了统一DFA的事件处理，简化DFA模型。可以在一个ID识别完成时，判断它是不是关键字或BOOLEAN类型。
  * 上述DFA只画出了识别一个Token时的状态转换
  * 写程序时，考虑到方便处理并没有将Integer的识别分为InINT1和InINT2两个状态，而是统一按InINT1的状态处理，如果检测到Token的值以0开头且长度不为1时，当出错处理。
  * 上述DFA并没有画出出错情况以及从错误中恢复，但程序中考虑到了。