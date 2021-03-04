#pragma once

enum ErrType {
    OK,
    ErrorImage,
    ErrorImageNull,
    ErrorTotalLine,
    ErrorColor,
    ErrorWid,
    ErrorDir,
    ErrorItem,
    ErrorFilePath,
    ErrorLoadJsonArray,
    Error
};

enum itemType {
    ItemPath = 0,
    ItemPoly = 1
};
