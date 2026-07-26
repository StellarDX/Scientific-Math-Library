/**
    @file 整数乘法器的长征.h
    @defgroup Multipliers 整数乘法器的长征
    @ingroup IPN

    @details 早在几千年前，商朝人就开始用十进制计数，四则运算也就应运而生。但那时的数学刚刚成
    型，没有形成系统性的口诀。西周时期，第一本数学著作《周髀算经》出现标志着当时的数学已经得到
    了巨大的进步。其中有一段记载是这样的：

    >昔者周公问于商高曰：窃闻乎大夫善数也。请问古者包牺立周天历度，夫天不可阶而升；地不可得尺
    >寸而度，请问数安从出？商高曰：数之法，出于圆方，圆出于方，方出于矩，矩出于九九八十一。故
    >折矩，以为句广三，股修四，径隅五。既方之，外半其一矩，环而共盘。得成三四五。两矩共长二十
    >有五。是谓积矩。故禹之所以治天下者。此数之所生也。

    这是一段西周数学家殷高与姬旦的一段对话，详细描述了数学的起源，其中“九九八十一”很可能描述的
    是最早的“九九口诀”，也就是说，乘法早在那时起就已经出现。另外，西汉学者韩婴在其著作《韩诗外
    传》中描述了春秋时期齐桓公姜小白“庭燎求贤”的故事，原文如下：

    >齐桓公设庭燎，为便人欲造见者，期年而士不至。于是东野有以九九见者，桓公使戏之曰：“九九足以
    >见乎？”鄙人曰：“臣闻君设庭燎以待士，期年而士不至。夫士之所以不至者，君、天下之贤君也，四
    >方之士皆自以不及君，故不至也。夫九九、薄能耳，而君犹礼之，况贤于九九者乎！夫太山不让砾石，
    >江海不辞小流，所以成其大也。诗曰：‘先民有言，询于刍荛。’博谋也。”桓公曰：“善。”乃固礼之。
    >期月，四方之士相导而至矣。诗曰：“自堂徂基，自羊徂牛。”以小成大。

    故事中提到在当时“九九之能”，也就是10以内乘法，已经成为大多数人都能够掌握的基本技能了。这点
    在春秋时期的很多文献当中也都有类似的记载，如《管子·地员》的一段记载：

    >夫管仲之匡天下也，其施七尺。\
    >渎田悉徙，五种无不宜，其立后而手实。其木宜蚖、菕与杜、松，其草宜楚棘。见是土也，命之曰五施，
    >五七三十五尺而至于泉。呼音中角。其水仓，其民强。\
    >赤垆，历强肥，五种无不宜。其麻白，其布黄，其草宜白茅与雚，其木宜赤棠。见是土也，命之曰四施，
    >四七二十八尺而至于泉。呼音中商。其水白而甘，其民寿。\
    >黄唐，无宜也，唯宜黍秫也。宜县泽。行廧落，地润数毁，难以立邑置廧。其草宜黍秫与茅，其木宜櫄、
    >桑。见是土也，命之曰三施，三七二十一尺而至于泉。呼音中宫。其泉黄而糗，流徙。\
    >斥埴，宜大菽与麦。其草宜萯、雚，其木宜杞。见是土也，命之曰再施，二七一十四尺而至于泉。呼音
    >中羽。其泉咸，水流徙。\
    >黑埴，宜稻麦。其草宜苹、蓨，其木宜白棠。见是土也，命之曰一施，七尺而至于泉。呼音中徵。其水
    >黑而苦。\
    >凡听徵，如负猪豕觉而骇。凡听羽，如鸣马在野。凡听宫，如牛鸣窌中。凡听商，如离群羊。凡听角，
    >如雉登木以鸣，音疾以清。凡将起五音凡首，先主一而三之，四开以合九九，以是生黄钟小素之首，以
    >成宫。三分而益之以一，为百有八，为徵。不无有三分而去其乘，适足，以是生商。有三分，而复于其
    >所，以是成羽。有三分，去其乘，适足，以是成角。\
    >坟延者，六施，六七四十二尺而至于泉。陕之芳七施，七七四十九尺而至于泉。祀陕八施，七八五十六
    >尺而至于泉。杜陵九施，七九六十三尺而至于泉。延陵十施，七十尺而至于泉。环陵十一施，七十七尺
    >而至于泉。蔓山十二施，八十四尺而至于泉。付山十三施，九十一尺而至于泉。付山白徒十四施，九十
    >八尺而至于泉。中陵十五施，百五尺而至于泉。青山十六施，百一十二尺而至于泉，青龙之所居，庚泥，
    >不可得泉，赤壤磝山十七施，百一十九尺而至于泉，其下清商，不可得泉。□山白壤十八施，百二十六
    >尺而至于泉，其下骈石，不可得泉。徙山十九施，百三十三尺而至于泉，其下有灰壤，不可得泉。高陵
    >土山二十施，百四十尺而至于泉。

    《管子·地员》是中国最早的土壤分类学和生物学著作，同时还将“三分损益”推导出的“五音”系统与每一
    种土壤进行对应。文中大量使用“九九表”中的语句来概括它们的数值，可以得知乘法在那时已经进入生
    活的方方面面。
    
    战国时期出现了最早的计算器——《算表》，被认为是世界上最早的十进制乘法器。它由21根长43.5cm，
    宽1.2cm的竹简组成，其中20根的上下两端各有一孔，剩下一根从上到下有20个孔，18条红线和3条编
    绳将每条竹简分成20段，与竹简本身交叉呈现为一张20行21列的表格。其中每一个单元格都包含一个数
    字，表格内容如下：

    <table>
        <tr><th>刖</th><th>一</th><th>二</th><th>三</th><th>四</th><th>五</th><th>六</th><th>七</th><th>八</th><th>九</th><th>十</th><th>廿</th><th>卅</th><th>四十</th><th>五十</th><th>六十</th><th>七十</th><th>八十</th><th>九十</th><th></th><th></th></tr>
        <tr><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th><th>○</th></tr>
        <tr><td>四十五</td><td>九十</td><td>百八十</td><td>二百七十</td><td>三百六十</td><td>四百五十</td><td>五百四十</td><td>六百卅</td><td>七百廿</td><td>八百十</td><td>九百</td><td>千八百</td><td>二千七百</td><td>三千六百</td><td>四千五百</td><td>五千四百</td><td>六千三百</td><td>七千二百</td><td>八千一百</td><th>○</th><th>九十</th></tr>
        <tr><td>四十</td><td>八十</td><td>百六十</td><td>二百四十</td><td>三百二十</td><td>四百</td><td>四百八十</td><td>五百六十</td><td>六百四十</td><td>七百廿</td><td>八百</td><td>千六百</td><td>二千四百</td><td>三千二百</td><td>四千</td><td>四千八百</td><td>五千六百</td><td>六千四百</td><td>七千二百</td><th>○</th><th>八十</th></tr>
        <tr><td>卅五</td><td>七十</td><td>百四十</td><td>二百十</td><td>二百八十</td><td>三百五十</td><td>四百二十</td><td>四百九十</td><td>五百六十</td><td>六百卅</td><td>七百</td><td>千四百</td><td>二千一百</td><td>二千八百</td><td>三千五百</td><td>四千二百</td><td>四千九百</td><td>五千六百</td><td>六千三百</td><th>○</th><th>七十</th></tr>
        <tr><td>卅</td><td>六十</td><td>百廿</td><td>百八十</td><td>二百四十</td><td>三百</td><td>三百六十</td><td>四百廿</td><td>四百八十</td><td>五百四十</td><td>六百</td><td>千二百</td><td>千八百</td><td>二千四百</td><td>三千</td><td>三千六百</td><td>四千二百</td><td>四千八百</td><td>五千四百</td><th>○</th><th>六十</th></tr>
        <tr><td>廿五</td><td>五十</td><td>百</td><td>百五十</td><td>二百</td><td>二百五十</td><td>三百</td><td>三百五十</td><td>四百</td><td>四百五十</td><td>五百</td><td>千</td><td>千五百</td><td>二千</td><td>二千五百</td><td>三千</td><td>三千五百</td><td>四千</td><td>四千五百</td><th>○</th><th>五十</th></tr>
        <tr><td>廿</td><td>四十</td><td>八十</td><td>百廿</td><td>百六十</td><td>二百</td><td>二百四十</td><td>二百八十</td><td>三百廿</td><td>三百六十</td><td>四百</td><td>八百</td><td>千二百</td><td>千六百</td><td>二千</td><td>二千四百</td><td>二千八百</td><td>三千二百</td><td>三千六百</td><th>○</th><th>四十</th></tr>
        <tr><td>十五</td><td>卅</td><td>六十</td><td>九十</td><td>百廿</td><td>百五十</td><td>百八十</td><td>二百十</td><td>二百四十</td><td>二百七十</td><td>三百</td><td>六百</td><td>九百</td><td>千二百</td><td>千五百</td><td>千八百</td><td>二千一百</td><td>二千四百</td><td>二千七百</td><th>○</th><th>卅</th></tr>
        <tr><td>十</td><td>廿</td><td>四十</td><td>六十</td><td>八十</td><td>百</td><td>百二十</td><td>百四十</td><td>百六十</td><td>百八十</td><td>二百</td><td>四百</td><td>六百</td><td>八百</td><td>千</td><td>千二百</td><td>千四百</td><td>千六百</td><td>千八百</td><th>○</th><th>廿</th></tr>
        <tr><td>五</td><td>十</td><td>廿</td><td>卅</td><td>四十</td><td>五十</td><td>六十</td><td>七十</td><td>八十</td><td>九十</td><td>百</td><td>二百</td><td>三百</td><td>四百</td><td>五百</td><td>六百</td><td>七百</td><td>八百</td><td>九百</td><th>○</th><th>十</th></tr>
        <tr><td>四刖</td><td>九</td><td>十八</td><td>廿七</td><td>三十六</td><td>四十五</td><td>五十四</td><td>六十三</td><td>七十二</td><td>八十一</td><td>九十</td><td>百八十</td><td>二百七十</td><td>三百六十</td><td>四百五十</td><td>五百四十</td><td>六百卅</td><td>七百廿</td><td>八百一十</td><th>○</th><th>九</th></tr>
        <tr><td>四</td><td>八</td><td>十六</td><td>廿四</td><td>三十二</td><td>四十</td><td>四十八</td><td>五十六</td><td>六十四</td><td>七十二</td><td>八十</td><td>百六十</td><td>二百四十</td><td>三百廿</td><td>四百</td><td>四百八十</td><td>五百六十</td><td>六百四十</td><td>七百二十</td><th>○</th><th>八</th></tr>
        <tr><td>三刖</td><td>七</td><td>十四</td><td>廿一</td><td>二十八</td><td>卅五</td><td>四十二</td><td>四十九</td><td>五十六</td><td>六十三</td><td>七十</td><td>百四十</td><td>二百十</td><td>二百八十</td><td>三百五十</td><td>四百廿</td><td>四百九十</td><td>五百六十</td><td>六百卅</td><th>○</th><th>七</th></tr>
        <tr><td>三</td><td>六</td><td>十二</td><td>十八</td><td>二十四</td><td>卅</td><td>三十六</td><td>四十二</td><td>四十八</td><td>五十四</td><td>六十</td><td>百廿</td><td>百八十</td><td>二百四十</td><td>三百</td><td>三百六十</td><td>四百廿</td><td>四百八十</td><td>五百四十</td><th>○</th><th>六</th></tr>
        <tr><td>二刖</td><td>五</td><td>十</td><td>十五</td><td>廿</td><td>廿五</td><td>三十</td><td>三十五</td><td>四十</td><td>四十五</td><td>五十</td><td>百</td><td>百五十</td><td>二百</td><td>二百五十</td><td>三百</td><td>三百五十</td><td>四百</td><td>四百五十</td><th>○</th><th>五</th></tr>
        <tr><td>二</td><td>四</td><td>八</td><td>十二</td><td>十六</td><td>廿</td><td>廿四</td><td>二十八</td><td>卅二</td><td>三十六</td><td>四十</td><td>八十</td><td>百二十</td><td>百六十</td><td>二百</td><td>二百四十</td><td>二百八十</td><td>三百廿</td><td>三百六十</td><th>○</th><th>四</th></tr>
        <tr><td>一刖</td><td>三</td><td>六</td><td>九</td><td>十二</td><td>十五</td><td>十八</td><td>二十一</td><td>廿四</td><td>廿七</td><td>卅</td><td>六十</td><td>九十</td><td>百廿</td><td>百五十</td><td>百八十</td><td>二百一十</td><td>二百四十</td><td>二百七十</td><th>○</th><th>三</th></tr>
        <tr><td>一</td><td>二</td><td>四</td><td>六</td><td>八</td><td>十</td><td>十二</td><td>十四</td><td>十六</td><td>十八</td><td>廿</td><td>四十</td><td>六十</td><td>八十</td><td>百</td><td>百廿</td><td>百四十</td><td>百六十</td><td>百八十</td><th>○</th><th>二</th></tr>
        <tr><td>刖</td><td>一</td><td>二</td><td>三</td><td>四</td><td>五</td><td>六</td><td>七</td><td>八</td><td>九</td><td>十</td><td>廿</td><td>三十</td><td>四十</td><td>五十</td><td>六十</td><td>七十</td><td>八十</td><td>九十</td><th>○</th><th>一</th></tr>
        <tr><td>釮</td><td>刖</td><td>一</td><td>一刖</td><td>二</td><td>二刖</td><td>三</td><td>三刖</td><td>四</td><td>四刖</td><td>五</td><td>十</td><td>十五</td><td>廿</td><td>廿五</td><td>卅</td><td>卅五</td><td>四十</td><td>四十五</td><th>○</th><th>刖</th></tr>
    </table>

    从表的内容可以看出，《算表》的定位是能够计算所有乘数<100且带有1/2的乘法。其中乘数<10的所有
    乘法结果均可以在表格中直接查出，10<=乘数<100以及乘数带1/2的乘法可以通过特殊操作简介计算。
    以计算12*35为例，先把两个乘数分解为十位和各位，也就是12写成10+2，35写成30+5，随后在表格中
    找到10和2对应的列，30和5对应的行，行列相交得到4个交点格，将交点格中的数全部加起来，就可以
    得到300+60+50+10=420，也就是式(10+2)*(30+5)展开后得到的结果。而对于一些带有1/2的乘法，也
    可以用相同的算法计算，如32.5*45.5，先把32.5分解成30+2+1/2，45.5分解成40+5+1/2，在表中找
    到30，2和1/2对应的列，40，5和1/2对应的行，行列相交得到9个交点格，全部加起来，即
    
        32.5*45.5
        => (30+2+1/2)*(40+5+1/2)
        => 1200+150+15+80+10+1+20+2.5+0.25
        => 1478.75

    在当时的背景来看，那时算盘还没出现，数学计算主要是靠算筹来完成的。在整数的四则运算中，算筹
    可以非常简单的实现加法和减法的计算，但一遇到10以上的乘法，需要的算筹数量就会非常多，操作也
    会更为复杂。并且，算筹还有一个问题就是难以保留历史记录，而《算表》作为功能强大的乘法器，它
    利用乘法分配律将复杂的乘法化为简单的加法，还能非常直观的获取大多数常用乘法运算的结果，使得
    乘法运算的速度和便捷性都得到了史诗级的提升。
    
    2002年6月，湖北湘西土家族苗族自治州龙山县里耶镇挖出了一堆秦简，它们是秦始皇二十五年（公元
    前222年）至秦二世二年（前208年）洞庭郡迁陵县遗留的公文档案，内容涉及政治、军事、民族、经济、
    法律、文化、邮传、地理等；其文书形式包括书传类、律令类、録课类、簿籍类、符券类、检楬类、历
    谱类、九九术和药方、里程书、习字简等。其中有一枚长约22厘米的木牍上，记载了目前已知最早的
    “九九乘法表”，全文如下：

    >九九八十一，八九七十二，七九六十三，六九五十四，五九卌五，四九卅六，三九廿七，二九十八。
    >八八六十四，七八五十六，六八卌八，五八卌，四八卅二，三八廿四，二八十六。
    >七七卌九， 六七卌二，五七卅五，四七廿八，三七廿一，二七十四。
    >六六卅六，五六卅，四六廿四，三六十八，二六十二。
    >五五廿五，四五廿，三五十五，二五而十。
    >四四十六，三四十二，二四而八。
    >三三而九，二三而六，二二而四。
    >一一而二，二半而一，凡千一百一十三字。（注：整个表里的乘积加起来是1113）

    可以看出这一版本与现行版本相比，它并不是从“一一得一”开始，而是从“九九八十一”为起点，降序排
    列，并且省略了乘数为1的情况，因为1乘以任何数得到其本身。另外，它的最后还包含了“二半而一”这
    样的分数运算，证实了当时已存在分数的概念与应用。

    @par 参考文献
    [1] 饶权,张柏春.格致·考工·源流:中国古代科技发明创造[M].北京大学出版社,2020.
    [2] 王焕林.里耶秦简九九表初探[J].吉首大学学报(社会科学版), 2006(01):51-56.
        DOI:CNKI:SUN:JSDX.0.2006-01-007.
    [3]	Granlund T, The GMP development team. GNU MP: The GNU Multiple 
        Precision Arithmetic Library[C]. 5.0.5. 2012.
*/

#pragma once

#ifndef __Multiplier__
#define __Multiplier__

#include <StellarMath/StellarDX-GMP/SMLDefs.h>

#define _MULTIPLIER_BEGIN namespace Multipliers {
#define _MULTIPLIER_END }
#define _MULTIPLIER Multipliers::

_ALU_BEGIN

/**
 * @brief 
 * 
 * @param DX 
 * @param AX 
 * @param BX 
 */
void MULADC(BlockArrayView DX, BlockArraySrcView AX, BlockType BX, BlockType* CF = nullptr);

/**
 * @brief 
 * 
 * @param DX 
 * @param AX 
 * @param BX 
 */
void MULSBB(BlockArrayView DX, BlockArraySrcView AX, BlockType BX, BlockType* CF = nullptr);

_MULTIPLIER_BEGIN

/**
    @brief 竖式乘法器
    @ingroup Multipliers

    @details 上回注释中说到，早在战国时期，我们就已经开始用算筹计算乘法，具体算法是将计算区域
    从上到下均分为三份，两个乘数摆在上下两边，然后在中间进行计算。以183*26为例，把26摆在上位，
    183摆在下位，上位乘数的十位对齐下位乘数的个位，然后从高位开始算，先把26十位上的2，也就是20
    与183相乘，得3660，摆在中间，结果的个位与下位乘数的个位对齐，随后移除上位乘数的十位，也就
    是26里头的那个2，将剩下的6左移一位，对齐下位乘数的个位，继续把剩下的6与183相乘，得1098，
    加到中间的结果上，得到最终计算结果4758。从筹算的计算过程不难看出，它与如今的竖式乘法极为相
    似，因此它被认为是竖式乘法最初的原型。

    意大利数学家Luca Pacioli在他的文献《Summa de arithmetica, geometria, proportioni et 
    proportionalita》中收录了印度数学家Bhāskara提出的一种乘法运算体系，其算法与现在的竖式乘法
    已基本相同，只是数位对齐的方式略有不同。现在我们中小学教的这种竖式乘法是文艺复兴时期出现的。

    当然，现在我们在计算机中使用乘法时，基本不会关心底层是如何实现的。事实上a*b这一步操作虽然
    看起来一瞬间就能完成，但事实上在CPU当中就是执行了一个竖式乘法器，只不过把十进制的乘法变成
    了二进制的乘法。不过竖式乘法器也有它的局限性，就是它的时间复杂度较高，因为它的算法是按照顺
    序把一个乘数的每一位或每一块与另一个乘数的所有位或所有块相乘后加起来（进位处理），这样得到
    的时间复杂度就是O(n^2)。这意味着这一算法在位数或块数很小的时候速度还能接受，当块数一多计
    算量就会炸，因此如何优化乘法器的算法到目前仍是一个待解决的问题。

    @par 参考文献
    [1]	Pacioli L. Summa de Arithmetica geometria proportioni : et 
        proportionalita[M/OL]. Paganino de Paganini, 1523. 
        https://books.google.com.hk/books?id=iqgPe49fhrsC.
    [2] 张建妮.基于FPGA的8位移位相加型硬件乘法器的设计[J].智能计算机与应用, 2014,
        4(4):4.DOI:10.3969/j.issn.2095-2163.2014.04.025.

    @fn void GMP_SingleBlkLongMultiplier(BlockArrayView DST, BlockArraySrcView AX, BlockType BX)
    @param DST 积
    @param AX  乘数1
    @param BX  乘数2
    
    @fn GMP_LongMultiplier(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX)
    @param DST 积
    @param AX  乘数1
    @param BX  乘数2
*/
void GMP_SingleBlkLongMultiplier(BlockArrayView DST, BlockArraySrcView AX, BlockType BX);
void GMP_LongMultiplier(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX);

/**
    @brief 图姆-库克乘法器
    @ingroup Multipliers

    
 */
__interface ToomCookMultipliers
{
    // TODO...
};

/**
    @brief FFT乘法器
    @ingroup Multipliers
 */
class FFTMultiplier
{
    // TODO...
};

_MULTIPLIER_END

using SingleBlkMulFuncType = void(BlockArrayView DST, BlockArraySrcView AX, BlockType BX);
using MultiplierFuncType = void(BlockArrayView DST, BlockArraySrcView AX, BlockArraySrcView BX);
using SingleBlkMulFuncPtr = SingleBlkMulFuncType*;
using MultiplierFuncPtr = MultiplierFuncType*;

_ALU_END

#endif