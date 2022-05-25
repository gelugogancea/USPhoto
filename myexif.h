/*USPHOTO UPLOAD PICTURES SERVER
Copyright G.NET Software 1991-2004

Modul pentru citirea informatiilor EXIF 
*/
#ifndef _MY_EXIF_
#define _MY_EXIF_

#include "usprotocol.h"
//#define EXIF_HEADER		{0x45,0x78,0x69,0x66,0x00,0x00}			//lungimea totala este 6 octeti
//#define TIFF_HEADER		{0x49,0x49,0x2A,0x00,0x08,0x00,0x00,0x00}	//lungimea totala este de 8 octeti 
#define JPEG_SOI		0xFFD8		
#define APP0_MARKER		0xFFE0
#define APP1_MARKER		0xFFE1

/* Describes tag values */
#define TAG_GPS_VERSION_ID              0x0000
#define TAG_GPS_LATITUDE_REF            0x0001
#define TAG_GPS_LATITUDE                0x0002
#define TAG_GPS_LONGITUDE_REF           0x0003
#define TAG_GPS_LONGITUDE               0x0004
#define TAG_GPS_ALTITUDE_REF            0x0005
#define TAG_GPS_ALTITUDE                0x0006
#define TAG_GPS_TIME_STAMP              0x0007
#define TAG_GPS_SATELLITES              0x0008
#define TAG_GPS_STATUS                  0x0009
#define TAG_GPS_MEASURE_MODE            0x000A
#define TAG_GPS_DOP                     0x000B
#define TAG_GPS_SPEED_REF               0x000C
#define TAG_GPS_SPEED                   0x000D
#define TAG_GPS_TRACK_REF               0x000E
#define TAG_GPS_TRACK                   0x000F
#define TAG_GPS_IMG_DIRECTION_REF       0x0010
#define TAG_GPS_IMG_DIRECTION           0x0011
#define TAG_GPS_MAP_DATUM               0x0012
#define TAG_GPS_DEST_LATITUDE_REF       0x0013
#define TAG_GPS_DEST_LATITUDE           0x0014
#define TAG_GPS_DEST_LONGITUDE_REF      0x0015
#define TAG_GPS_DEST_LONGITUDE          0x0016
#define TAG_GPS_DEST_BEARING_REF        0x0017
#define TAG_GPS_DEST_BEARING            0x0018
#define TAG_GPS_DEST_DISTANCE_REF       0x0019
#define TAG_GPS_DEST_DISTANCE           0x001A
#define TAG_GPS_PROCESSING_METHOD       0x001B
#define TAG_GPS_AREA_INFORMATION        0x001C
#define TAG_GPS_DATE_STAMP              0x001D
#define TAG_GPS_DIFFERENTIAL            0x001E
#define TAG_TIFF_COMMENT                0x00FE /* SHOUDLNT HAPPEN */
#define TAG_NEW_SUBFILE                 0x00FE /* New version of subfile tag */
#define TAG_SUBFILE_TYPE                0x00FF /* Old version of subfile tag */
#define TAG_IMAGEWIDTH                  0x0100
#define TAG_IMAGEHEIGHT                 0x0101
#define TAG_BITS_PER_SAMPLE             0x0102
#define TAG_COMPRESSION                 0x0103
#define TAG_PHOTOMETRIC_INTERPRETATION  0x0106
#define TAG_TRESHHOLDING                0x0107
#define TAG_CELL_WIDTH                  0x0108
#define TAG_CELL_HEIGHT                 0x0109
#define TAG_FILL_ORDER                  0x010A
#define TAG_DOCUMENT_NAME               0x010D
#define TAG_IMAGE_DESCRIPTION           0x010E
#define TAG_MAKE                        0x010F
#define TAG_MODEL                       0x0110
#define TAG_STRIP_OFFSETS               0x0111
#define TAG_ORIENTATION                 0x0112
#define TAG_SAMPLES_PER_PIXEL           0x0115
#define TAG_ROWS_PER_STRIP              0x0116
#define TAG_STRIP_BYTE_COUNTS           0x0117
#define TAG_MIN_SAMPPLE_VALUE           0x0118
#define TAG_MAX_SAMPLE_VALUE            0x0119
#define TAG_X_RESOLUTION                0x011A
#define TAG_Y_RESOLUTION                0x011B
#define TAG_PLANAR_CONFIGURATION        0x011C
#define TAG_PAGE_NAME                   0x011D
#define TAG_X_POSITION                  0x011E
#define TAG_Y_POSITION                  0x011F
#define TAG_FREE_OFFSETS                0x0120
#define TAG_FREE_BYTE_COUNTS            0x0121
#define TAG_GRAY_RESPONSE_UNIT          0x0122
#define TAG_GRAY_RESPONSE_CURVE         0x0123
#define TAG_RESOLUTION_UNIT             0x0128
#define TAG_PAGE_NUMBER                 0x0129
#define TAG_TRANSFER_FUNCTION           0x012D
#define TAG_SOFTWARE                    0x0131
#define TAG_DATETIME                    0x0132
#define TAG_ARTIST                      0x013B
#define TAG_HOST_COMPUTER               0x013C
#define TAG_PREDICTOR                   0x013D
#define TAG_WHITE_POINT                 0x013E
#define TAG_PRIMARY_CHROMATICITIES      0x013F
#define TAG_COLOR_MAP                   0x0140
#define TAG_HALFTONE_HINTS              0x0141
#define TAG_TILE_WIDTH                  0x0142
#define TAG_TILE_LENGTH                 0x0143
#define TAG_TILE_OFFSETS                0x0144
#define TAG_TILE_BYTE_COUNTS            0x0145
#define TAG_SUB_IFD                     0x014A
#define TAG_INK_SETMPUTER               0x014C
#define TAG_INK_NAMES                   0x014D
#define TAG_NUMBER_OF_INKS              0x014E
#define TAG_DOT_RANGE                   0x0150
#define TAG_TARGET_PRINTER              0x0151
#define TAG_EXTRA_SAMPLE                0x0152
#define TAG_SAMPLE_FORMAT               0x0153
#define TAG_S_MIN_SAMPLE_VALUE          0x0154
#define TAG_S_MAX_SAMPLE_VALUE          0x0155
#define TAG_TRANSFER_RANGE              0x0156
#define TAG_JPEG_TABLES                 0x015B
#define TAG_JPEG_PROC                   0x0200
#define TAG_JPEG_INTERCHANGE_FORMAT     0x0201
#define TAG_JPEG_INTERCHANGE_FORMAT_LEN 0x0202
#define TAG_JPEG_RESTART_INTERVAL       0x0203
#define TAG_JPEG_LOSSLESS_PREDICTOR     0x0205
#define TAG_JPEG_POINT_TRANSFORMS       0x0206
#define TAG_JPEG_Q_TABLES               0x0207
#define TAG_JPEG_DC_TABLES              0x0208
#define TAG_JPEG_AC_TABLES              0x0209
#define TAG_YCC_COEFFICIENTS            0x0211
#define TAG_YCC_SUB_SAMPLING            0x0212
#define TAG_YCC_POSITIONING             0x0213
#define TAG_REFERENCE_BLACK_WHITE       0x0214

#define TAG_COPYRIGHT                   0x8298
#define TAG_EXPOSURETIME                0x829A
#define TAG_FNUMBER                     0x829D
#define TAG_EXIF_IFD_POINTER            0x8769
#define TAG_ICC_PROFILE                 0x8773
#define TAG_EXPOSURE_PROGRAM            0x8822
#define TAG_SPECTRAL_SENSITY            0x8824
#define TAG_GPS_IFD_POINTER             0x8825
#define TAG_ISOSPEED                    0x8827
#define TAG_OPTOELECTRIC_CONVERSION_F   0x8828
/* 0x8829 - 0x882b */
#define TAG_EXIFVERSION                 0x9000
#define TAG_DATE_TIME_ORIGINAL          0x9003
#define TAG_DATE_TIME_DIGITIZED         0x9004
#define TAG_COMPONENT_CONFIG            0x9101
#define TAG_COMPRESSED_BITS_PER_PIXEL   0x9102
#define TAG_SHUTTERSPEED                0x9201
#define TAG_APERTURE                    0x9202
#define TAG_BRIGHTNESS_VALUE            0x9203
#define TAG_EXPOSURE_BIAS_VALUE         0x9204
#define TAG_MAX_APERTURE                0x9205
#define TAG_SUBJECT_DISTANCE            0x9206
#define TAG_METRIC_MODULE               0x9207
#define TAG_LIGHT_SOURCE                0x9208
#define TAG_FLASH                       0x9209
#define TAG_FOCAL_LENGTH                0x920A
/* 0x920B - 0x920D */
/* 0x9211 - 0x9216 */
#define TAG_SUBJECT_AREA                0x9214
#define TAG_MAKER_NOTE                  0x927C
#define TAG_USERCOMMENT                 0x9286
#define TAG_SUB_SEC_TIME                0x9290
#define TAG_SUB_SEC_TIME_ORIGINAL       0x9291
#define TAG_SUB_SEC_TIME_DIGITIZED      0x9292
/* 0x923F */
/* 0x935C */
#define TAG_XP_TITLE                    0x9C9B
#define TAG_XP_COMMENTS                 0x9C9C
#define TAG_XP_AUTHOR                   0x9C9D
#define TAG_XP_KEYWORDS                 0x9C9E
#define TAG_XP_SUBJECT                  0x9C9F
#define TAG_FLASH_PIX_VERSION           0xA000
#define TAG_COLOR_SPACE                 0xA001
#define TAG_COMP_IMAGE_WIDTH            0xA002 /* compressed images only */
#define TAG_COMP_IMAGE_HEIGHT           0xA003
#define TAG_RELATED_SOUND_FILE          0xA004
#define TAG_INTEROP_IFD_POINTER         0xA005 /* IFD pointer */
#define TAG_FLASH_ENERGY                0xA20B
#define TAG_SPATIAL_FREQUENCY_RESPONSE  0xA20C
#define TAG_FOCALPLANE_X_RES            0xA20E
#define TAG_FOCALPLANE_Y_RES            0xA20F
#define TAG_FOCALPLANE_RESOLUTION_UNIT  0xA210
#define TAG_SUBJECT_LOCATION            0xA214
#define TAG_EXPOSURE_INDEX              0xA215
#define TAG_SENSING_METHOD              0xA217
#define TAG_FILE_SOURCE                 0xA300
#define TAG_SCENE_TYPE                  0xA301
#define TAG_CFA_PATTERN                 0xA302
#define TAG_CUSTOM_RENDERED             0xA401
#define TAG_EXPOSURE_MODE               0xA402
#define TAG_WHITE_BALANCE               0xA403
#define TAG_DIGITAL_ZOOM_RATIO          0xA404
#define TAG_FOCAL_LENGTH_IN_35_MM_FILM  0xA405
#define TAG_SCENE_CAPTURE_TYPE          0xA406
#define TAG_GAIN_CONTROL                0xA407
#define TAG_CONTRAST                    0xA408
#define TAG_SATURATION                  0xA409
#define TAG_SHARPNESS                   0xA40A
#define TAG_DEVICE_SETTING_DESCRIPTION  0xA40B
#define TAG_SUBJECT_DISTANCE_RANGE      0xA40C
#define TAG_IMAGE_UNIQUE_ID             0xA420

/* Internal */
#define TAG_NONE               			-1 
#define TAG_COMPUTED_VALUE     			-2
#define TAG_END_OF_LIST                 0xFFFD

#define NUM_FORMATS 13

#define TAG_FMT_BYTE       1
#define TAG_FMT_STRING     2
#define TAG_FMT_USHORT     3
#define TAG_FMT_ULONG      4
#define TAG_FMT_URATIONAL  5
#define TAG_FMT_SBYTE      6
#define TAG_FMT_UNDEFINED  7
#define TAG_FMT_SSHORT     8
#define TAG_FMT_SLONG      9
#define TAG_FMT_SRATIONAL 10
#define TAG_FMT_SINGLE    11
#define TAG_FMT_DOUBLE    12
#define TAG_FMT_IFD       13

typedef enum byte_align{
	INTEL_FORMAT=0,
	MOTOROLA_FORMAT
}MY_BYTE_ALIGN;

typedef const struct {
	unsigned short Tag;
	char *Desc;
} tag_info_type;

typedef tag_info_type  tag_info_array[];
typedef tag_info_type  *tag_table_type;

#define TAG_TABLE_END \
  {TAG_NONE,           "No tag value"},\
  {TAG_COMPUTED_VALUE, "Computed value"},\
  {TAG_END_OF_LIST,    ""}  /* Important for exif_get_tagname() IF value != "" functionresult is != false */

static tag_info_array tag_table_IFD = {
  { 0x000B, "ACDComment"},
  { 0x00FE, "NewSubFile"}, /* better name it 'ImageType' ? */
  { 0x00FF, "SubFile"},
  { 0x0100, "ImageWidth"},
  { 0x0101, "ImageLength"},
  { 0x0102, "BitsPerSample"},
  { 0x0103, "Compression"},
  { 0x0106, "PhotometricInterpretation"},
  { 0x010A, "FillOrder"},
  { 0x010D, "DocumentName"},
  { 0x010E, "ImageDescription"},
  { 0x010F, "Make"},
  { 0x0110, "Model"},
  { 0x0111, "StripOffsets"},
  { 0x0112, "Orientation"},
  { 0x0115, "SamplesPerPixel"},
  { 0x0116, "RowsPerStrip"},
  { 0x0117, "StripByteCounts"},
  { 0x0118, "MinSampleValue"},
  { 0x0119, "MaxSampleValue"},
  { 0x011A, "XResolution"},
  { 0x011B, "YResolution"},
  { 0x011C, "PlanarConfiguration"},
  { 0x011D, "PageName"},
  { 0x011E, "XPosition"},
  { 0x011F, "YPosition"},
  { 0x0120, "FreeOffsets"},
  { 0x0121, "FreeByteCounts"},
  { 0x0122, "GrayResponseUnit"},
  { 0x0123, "GrayResponseCurve"},
  { 0x0124, "T4Options"},
  { 0x0125, "T6Options"},
  { 0x0128, "ResolutionUnit"},
  { 0x0129, "PageNumber"},
  { 0x012D, "TransferFunction"},
  { 0x0131, "Software"},
  { 0x0132, "DateTime"},
  { 0x013B, "Artist"},
  { 0x013C, "HostComputer"},
  { 0x013D, "Predictor"},
  { 0x013E, "WhitePoint"},
  { 0x013F, "PrimaryChromaticities"},
  { 0x0140, "ColorMap"},
  { 0x0141, "HalfToneHints"},
  { 0x0142, "TileWidth"},
  { 0x0143, "TileLength"},
  { 0x0144, "TileOffsets"},
  { 0x0145, "TileByteCounts"},
  { 0x014A, "SubIFD"},
  { 0x014C, "InkSet"},
  { 0x014D, "InkNames"},
  { 0x014E, "NumberOfInks"},
  { 0x0150, "DotRange"},
  { 0x0151, "TargetPrinter"},
  { 0x0152, "ExtraSample"},
  { 0x0153, "SampleFormat"},
  { 0x0154, "SMinSampleValue"},
  { 0x0155, "SMaxSampleValue"},
  { 0x0156, "TransferRange"},
  { 0x0157, "ClipPath"},
  { 0x0158, "XClipPathUnits"},
  { 0x0159, "YClipPathUnits"},
  { 0x015A, "Indexed"},
  { 0x015B, "JPEGTables"},
  { 0x015F, "OPIProxy"},
  { 0x0200, "JPEGProc"},
  { 0x0201, "JPEGInterchangeFormat"},
  { 0x0202, "JPEGInterchangeFormatLength"},
  { 0x0203, "JPEGRestartInterval"},
  { 0x0205, "JPEGLosslessPredictors"},
  { 0x0206, "JPEGPointTransforms"},
  { 0x0207, "JPEGQTables"},
  { 0x0208, "JPEGDCTables"},
  { 0x0209, "JPEGACTables"},
  { 0x0211, "YCbCrCoefficients"},
  { 0x0212, "YCbCrSubSampling"},
  { 0x0213, "YCbCrPositioning"},
  { 0x0214, "ReferenceBlackWhite"},
  { 0x02BC, "ExtensibleMetadataPlatform"}, /* XAP: Extensible Authoring Publishing, obsoleted by XMP: Extensible Metadata Platform */
  { 0x0301, "Gamma"}, 
  { 0x0302, "ICCProfileDescriptor"}, 
  { 0x0303, "SRGBRenderingIntent"}, 
  { 0x0320, "ImageTitle"}, 
  { 0x5001, "ResolutionXUnit"}, 
  { 0x5002, "ResolutionYUnit"}, 
  { 0x5003, "ResolutionXLengthUnit"}, 
  { 0x5004, "ResolutionYLengthUnit"}, 
  { 0x5005, "PrintFlags"}, 
  { 0x5006, "PrintFlagsVersion"}, 
  { 0x5007, "PrintFlagsCrop"}, 
  { 0x5008, "PrintFlagsBleedWidth"}, 
  { 0x5009, "PrintFlagsBleedWidthScale"}, 
  { 0x500A, "HalftoneLPI"}, 
  { 0x500B, "HalftoneLPIUnit"}, 
  { 0x500C, "HalftoneDegree"}, 
  { 0x500D, "HalftoneShape"}, 
  { 0x500E, "HalftoneMisc"}, 
  { 0x500F, "HalftoneScreen"}, 
  { 0x5010, "JPEGQuality"}, 
  { 0x5011, "GridSize"}, 
  { 0x5012, "ThumbnailFormat"}, 
  { 0x5013, "ThumbnailWidth"}, 
  { 0x5014, "ThumbnailHeight"}, 
  { 0x5015, "ThumbnailColorDepth"}, 
  { 0x5016, "ThumbnailPlanes"}, 
  { 0x5017, "ThumbnailRawBytes"}, 
  { 0x5018, "ThumbnailSize"}, 
  { 0x5019, "ThumbnailCompressedSize"}, 
  { 0x501A, "ColorTransferFunction"}, 
  { 0x501B, "ThumbnailData"}, 
  { 0x5020, "ThumbnailImageWidth"}, 
  { 0x5021, "ThumbnailImageHeight"}, 
  { 0x5022, "ThumbnailBitsPerSample"}, 
  { 0x5023, "ThumbnailCompression"}, 
  { 0x5024, "ThumbnailPhotometricInterp"}, 
  { 0x5025, "ThumbnailImageDescription"}, 
  { 0x5026, "ThumbnailEquipMake"}, 
  { 0x5027, "ThumbnailEquipModel"}, 
  { 0x5028, "ThumbnailStripOffsets"}, 
  { 0x5029, "ThumbnailOrientation"}, 
  { 0x502A, "ThumbnailSamplesPerPixel"}, 
  { 0x502B, "ThumbnailRowsPerStrip"}, 
  { 0x502C, "ThumbnailStripBytesCount"}, 
  { 0x502D, "ThumbnailResolutionX"}, 
  { 0x502E, "ThumbnailResolutionY"}, 
  { 0x502F, "ThumbnailPlanarConfig"}, 
  { 0x5030, "ThumbnailResolutionUnit"}, 
  { 0x5031, "ThumbnailTransferFunction"}, 
  { 0x5032, "ThumbnailSoftwareUsed"}, 
  { 0x5033, "ThumbnailDateTime"}, 
  { 0x5034, "ThumbnailArtist"}, 
  { 0x5035, "ThumbnailWhitePoint"}, 
  { 0x5036, "ThumbnailPrimaryChromaticities"}, 
  { 0x5037, "ThumbnailYCbCrCoefficients"}, 
  { 0x5038, "ThumbnailYCbCrSubsampling"}, 
  { 0x5039, "ThumbnailYCbCrPositioning"}, 
  { 0x503A, "ThumbnailRefBlackWhite"}, 
  { 0x503B, "ThumbnailCopyRight"}, 
  { 0x5090, "LuminanceTable"}, 
  { 0x5091, "ChrominanceTable"}, 
  { 0x5100, "FrameDelay"}, 
  { 0x5101, "LoopCount"}, 
  { 0x5110, "PixelUnit"}, 
  { 0x5111, "PixelPerUnitX"}, 
  { 0x5112, "PixelPerUnitY"}, 
  { 0x5113, "PaletteHistogram"}, 
  { 0x1000, "RelatedImageFileFormat"},
  { 0x800D, "ImageID"},
  { 0x80E3, "Matteing"},   /* obsoleted by ExtraSamples */
  { 0x80E4, "DataType"},   /* obsoleted by SampleFormat */
  { 0x80E5, "ImageDepth"},
  { 0x80E6, "TileDepth"},
  { 0x828D, "CFARepeatPatternDim"},
  { 0x828E, "CFAPattern"},
  { 0x828F, "BatteryLevel"},
  { 0x8298, "Copyright"},
  { 0x829A, "ExposureTime"},
  { 0x829D, "FNumber"},
  { 0x83BB, "IPTC/NAA"},
  { 0x84E3, "IT8RasterPadding"},
  { 0x84E5, "IT8ColorTable"},
  { 0x8649, "ImageResourceInformation"}, /* PhotoShop */
  { 0x8769, "Exif_IFD_Pointer"},
  { 0x8773, "ICC_Profile"},
  { 0x8822, "ExposureProgram"},
  { 0x8824, "SpectralSensity"},
  { 0x8828, "OECF"},
  { 0x8825, "GPS_IFD_Pointer"},
  { 0x8827, "ISOSpeedRatings"},
  { 0x8828, "OECF"},
  { 0x9000, "ExifVersion"},
  { 0x9003, "DateTimeOriginal"},
  { 0x9004, "DateTimeDigitized"},
  { 0x9101, "ComponentsConfiguration"},
  { 0x9102, "CompressedBitsPerPixel"},
  { 0x9201, "ShutterSpeedValue"},
  { 0x9202, "ApertureValue"},
  { 0x9203, "BrightnessValue"},
  { 0x9204, "ExposureBiasValue"},
  { 0x9205, "MaxApertureValue"},
  { 0x9206, "SubjectDistance"},
  { 0x9207, "MeteringMode"},
  { 0x9208, "LightSource"},
  { 0x9209, "Flash"},
  { 0x920A, "FocalLength"},
  { 0x920B, "FlashEnergy"},                 /* 0xA20B  in JPEG   */
  { 0x920C, "SpatialFrequencyResponse"},    /* 0xA20C    -  -    */
  { 0x920D, "Noise"},
  { 0x920E, "FocalPlaneXResolution"},       /* 0xA20E    -  -    */
  { 0x920F, "FocalPlaneYResolution"},       /* 0xA20F    -  -    */
  { 0x9210, "FocalPlaneResolutionUnit"},    /* 0xA210    -  -    */
  { 0x9211, "ImageNumber"},
  { 0x9212, "SecurityClassification"},
  { 0x9213, "ImageHistory"},
  { 0x9214, "SubjectLocation"},             /* 0xA214    -  -    */
  { 0x9215, "ExposureIndex"},               /* 0xA215    -  -    */
  { 0x9216, "TIFF/EPStandardID"},
  { 0x9217, "SensingMethod"},               /* 0xA217    -  -    */
  { 0x923F, "StoNits"},
  { 0x927C, "MakerNote"},
  { 0x9286, "UserComment"},
  { 0x9290, "SubSecTime"},
  { 0x9291, "SubSecTimeOriginal"},
  { 0x9292, "SubSecTimeDigitized"},
  { 0x935C, "ImageSourceData"},             /* "Adobe Photoshop Document Data Block": 8BIM... */
  { 0x9c9b, "Title" },                      /* Win XP specific, Unicode  */
  { 0x9c9c, "Comments" },                   /* Win XP specific, Unicode  */
  { 0x9c9d, "Author" },                     /* Win XP specific, Unicode  */
  { 0x9c9e, "Keywords" },                   /* Win XP specific, Unicode  */
  { 0x9c9f, "Subject" },                    /* Win XP specific, Unicode, not to be confused with SubjectDistance and SubjectLocation */
  { 0xA000, "FlashPixVersion"},
  { 0xA001, "ColorSpace"},
  { 0xA002, "ExifImageWidth"},
  { 0xA003, "ExifImageLength"},
  { 0xA004, "RelatedSoundFile"},
  { 0xA005, "InteroperabilityOffset"},
  { 0xA20B, "FlashEnergy"},                 /* 0x920B in TIFF/EP */
  { 0xA20C, "SpatialFrequencyResponse"},    /* 0x920C    -  -    */
  { 0xA20D, "Noise"},
  { 0xA20E, "FocalPlaneXResolution"},    	/* 0x920E    -  -    */
  { 0xA20F, "FocalPlaneYResolution"},       /* 0x920F    -  -    */
  { 0xA210, "FocalPlaneResolutionUnit"},    /* 0x9210    -  -    */
  { 0xA211, "ImageNumber"},
  { 0xA212, "SecurityClassification"},
  { 0xA213, "ImageHistory"},
  { 0xA214, "SubjectLocation"},             /* 0x9214    -  -    */
  { 0xA215, "ExposureIndex"},               /* 0x9215    -  -    */
  { 0xA216, "TIFF/EPStandardID"},
  { 0xA217, "SensingMethod"},               /* 0x9217    -  -    */
  { 0xA300, "FileSource"},
  { 0xA301, "SceneType"},
  { 0xA302, "CFAPattern"},
  { 0xA401, "CustomRendered"},
  { 0xA402, "ExposureMode"},
  { 0xA403, "WhiteBalance"},
  { 0xA404, "DigitalZoomRatio"},
  { 0xA405, "FocalLengthIn35mmFilm"},
  { 0xA406, "SceneCaptureType"},
  { 0xA407, "GainControl"},
  { 0xA408, "Contrast"},
  { 0xA409, "Saturation"},
  { 0xA40A, "Sharpness"},
  { 0xA40B, "DeviceSettingDescription"},
  { 0xA40C, "SubjectDistanceRange"},
  { 0xA420, "ImageUniqueID"},
  TAG_TABLE_END
};


#endif


