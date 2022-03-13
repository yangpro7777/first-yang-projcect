/**
 @file  company.h
 @brief Company header

 @verbatim
 File: company.h

 Desc: Company header

 Copyright (c) 2021 MainConcept GmbH or its affiliates. All rights reserved.

 MainConcept and its logos are registered trademarks of MainConcept GmbH or its affiliates.
 This software is protected by copyright law and international treaties. Unauthorized
 reproduction or distribution of any portion is prohibited by law.
 @endverbatim
 **/

#define NAME_PREFIX         demo_
#define COMPANY_PREFIX      "demo_"
#define COMPANY_NAME        "MainConcept GmbH"
#define COMPANY_WNAME       L"MainConcept GmbH"
#define COMPANY_SHORTNAME   "MainConcept (Demo)"
#define COMPANY_WSHORTNAME  L"MainConcept (Demo)"

#define COMPANY_WWW_        "www.mainconcept.com"
#define COMPANY_WWW_L       L"www.mainconcept.com"
#define COMPANY_WWW_T       TEXT(COMPANY_WWW_)
#define VENDOR_INFO_STRING  L"MainConcept GmbH http://www.mainconcept.com"

#define COMPANY_MARKER_NAME " "
#define COMPANY_MARKER_KEY  {0}

#define COMPLCNFG_REGFREE_NOLIMITS
#undef  COMPLCNFG_REGFREE_NOLOGO

#ifndef DEMO_LOGO
#define DEMO_LOGO
#endif
