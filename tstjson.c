/*
 *  Json.c
 *  Idiome
 *
 *  Created by Pierre-Jean ALBERT on Tue May 20 2015.
 *  Copyright (c) 2013 __An Intellectual Projet__. All rights reserved.
 *
 */

#include "json.h"

/**
		 */
int main (int argn, char* argc[]) {
    JsonParser* pJsonParser;

    pJsonParser = InitJsonParser("{ \"Search\" : 3 }", 0, false);
    if (!pJsonParser) {
        printf("Error Creating Json - 00 \n");
        exit(-1);
    }
    DumpJsonStruct(pJsonParser);

    pJsonParser = InitJsonParser("{ \"Search\" : 3, \"Field\" }", 0, false);
    if (!pJsonParser) {
        printf("Error Creating Json - 01 \n");
        exit(-1);
    }
    DumpJsonStruct(pJsonParser);

    pJsonParser = InitJsonParser("{ \"Search\" : 3, \"Field\" : [\"bali\", \"indonesia\"] }", 0, false);
    if (!pJsonParser) {
        printf("Error Creating Json - 02 \n");
        exit(-1);
    }


    if (GetJsonElementValue(pJsonParser, "Search"))
        printf("!!! Find 02 \n");
    else
        DumpJsonStruct(pJsonParser);

    printf("!!! Nb Element %d \n", GetJsonElementCount(pJsonParser, "Search"));

    pJsonParser = InitJsonParser("{ \"Search\" : 3, {     \"Employees\" : [{ \"firstName\":\"John\" , \"lastName\":\"Doe\" }, { \"firstName\":\"Anna\" , \"lastName\":\"Smith\" }, { \"firstName\":\"Peter\" , \"lastName\":\"Jones\" } ]} }", 0, false);
    if (!pJsonParser) {
        printf("Error Creating Json - 03 \n");
        exit(-1);
    }

    pJsonParser = InitJsonParser("{\"widget\": {    \"debug\": \"on\",    \"window\": {        \"title\": \"Sample Konfabulator Widget\",        \"name\": \"main_window\",        \"width\": 500,        \"height\": 500    },    \"image\": {         \"src\": \"Images/Sun.png\",        \"name\": \"sun1\",        \"hOffset\": 250,        \"vOffset\": 250,        \"alignment\": \"center\"    },    \"text\": {        \"data\": \"Click Here\",        \"size\": 36,        \"style\": \"bold\",        \"name\": \"text1\",        \"hOffset\": 250,        \"vOffset\": 100,        \"alignment\": \"center\",        \"onMouseUp\": \"sun1.opacity = (sun1.opacity / 100) * 90;\"    }}}", 0, false);
    if (!pJsonParser) {
        printf("Error Creating Json - 04 \n");
        exit(-1);
    }

    if (GetJsonElementValue(pJsonParser, "widget.window.title"))
        printf("!!! Find 04 \n");
    else
        DumpJsonStruct(pJsonParser);

    pJsonParser = InitJsonParser("{\"web-app\": {  \"servlet\": [       {      \"servlet-name\": \"cofaxCDS\",      \"servlet-class\": \"org.cofax.cds.CDSServlet\",      \"init-param\": {        \"configGlossary:installationAt\": \"Philadelphia, PA\",        \"configGlossary:adminEmail\": \"ksm@pobox.com\",        \"configGlossary:poweredBy\": \"Cofax\",        \"configGlossary:poweredByIcon\": \"/images/cofax.gif\",        \"configGlossary:staticPath\": \"/content/static\",        \"templateProcessorClass\": \"org.cofax.WysiwygTemplate\",        \"templateLoaderClass\": \"org.cofax.FilesTemplateLoader\",        \"templatePath\": \"templates\",        \"templateOverridePath\": \"\",  \
        \"defaultListTemplate\": \"listTemplate.htm\",        \"defaultFileTemplate\": \"articleTemplate.htm\",        \"useJSP\": false,        \"jspListTemplate\": \"listTemplate.jsp\",        \"jspFileTemplate\": \"articleTemplate.jsp\",        \"cachePackageTagsTrack\": 200,        \"cachePackageTagsStore\": 200,        \"cachePackageTagsRefresh\": 60,        \"cacheTemplatesTrack\": 100,        \"cacheTemplatesStore\": 50,        \"cacheTemplatesRefresh\": 15,        \"cachePagesTrack\": 200,        \"cachePagesStore\": 100,        \"cachePagesRefresh\": 10,        \"cachePagesDirtyRead\": 10,        \"searchEngineListTemplate\": \"forSearchEnginesList.htm\",        \
        \"searchEngineFileTemplate\": \"forSearchEngines.htm\",        \"searchEngineRobotsDb\": \"WEB-INF/robots.db\",        \"useDataStore\": true,        \"dataStoreClass\": \"org.cofax.SqlDataStore\",        \"redirectionClass\": \"org.cofax.SqlRedirection\",        \"dataStoreName\": \"cofax\",        \"dataStoreDriver\": \"com.microsoft.jdbc.sqlserver.SQLServerDriver\",        \"dataStoreUrl\": \"jdbc:microsoft:sqlserver://LOCALHOST:1433;DatabaseName=goon\",        \"dataStoreUser\": \"sa\",        \"dataStorePassword\": \"dataStoreTestQuery\",        \"dataStoreTestQuery\": \"SET NOCOUNT ON;select test='test';\",        \"dataStoreLogFile\": \"/usr/local/tomcat/logs/datastore.log\", \
        \"dataStoreInitConns\": 10,        \"dataStoreMaxConns\": 100,        \"dataStoreConnUsageLimit\": 100,        \"dataStoreLogLevel\": \"debug\",        \"maxUrlLength\": 500}},    {      \"servlet-name\": \"cofaxEmail\",      \"servlet-class\": \"org.cofax.cds.EmailServlet\",      \"init-param\": {      \"mailHost\": \"mail1\",      \"mailHostOverride\": \"mail2\"}},    {      \"servlet-name\": \"cofaxAdmin\",      \"servlet-class\": \"org.cofax.cds.AdminServlet\"},     {      \"servlet-name\": \"fileServlet\",      \"servlet-class\": \"org.cofax.cds.FileServlet\"},    {      \"servlet-name\": \"cofaxTools\",      \"servlet-class\": \"org.cofax.cms.CofaxToolsServlet\",      \"init-param\": { \
        \"templatePath\": \"toolstemplates/\",        \"log\": 1,        \"logLocation\": \"/usr/local/tomcat/logs/CofaxTools.log\",        \"logMaxSize\": \"\",        \"dataLog\": 1,        \"dataLogLocation\": \"/usr/local/tomcat/logs/dataLog.log\",        \"dataLogMaxSize\": \"\",        \"removePageCache\": \"/content/admin/remove?cache=pages&id=\",        \"removeTemplateCache\": \"/content/admin/remove?cache=templates&id=\",        \"fileTransferFolder\": \"/usr/local/tomcat/webapps/content/fileTransferFolder\",        \"lookInContext\": 1,        \"adminGroupID\": 4,        \"betaServer\": true}}],  \"servlet-mapping\": {    \"cofaxCDS\": \"/\",    \"cofaxEmail\": \"/cofaxutil/aemail/*\",  \
        \"cofaxAdmin\": \"/admin/*\",    \"fileServlet\": \"/static/*\",    \"cofaxTools\": \"/tools/*\"},   \"taglib\": {    \"taglib-uri\": \"cofax.tld\",    \"taglib-location\": \"/WEB-INF/tlds/cofax.tld\"}}}", 0, false);
    if (!pJsonParser) {
        printf("Error Creating Json - 05 \n");
        exit(-1);
    }
    DumpJsonStruct(pJsonParser);

    if (GetJsonElementValue(pJsonParser, "web-app.servlet.init-param.mailHost"))
        printf("!!! Find 05 \n");

    printf("!!! Nb Element %d \n", GetJsonElementCount(pJsonParser, "web-app.servlet.servlet-name"));


    printf("!!! Element 0 -> %s\n", GetJsonElementValueAt(pJsonParser, "web-app.servlet.servlet-name", 0));
    printf("!!! Element 1 -> %s\n", GetJsonElementValueAt(pJsonParser, "web-app.servlet.servlet-name", 1));
    printf("!!! Element 2 -> %s\n", GetJsonElementValueAt(pJsonParser, "web-app.servlet.servlet-name", 2));
    printf("!!! Element 3 -> %s\n", GetJsonElementValueAt(pJsonParser, "web-app.servlet.servlet-name", 3));
    printf("!!! Element 4 -> %s\n", GetJsonElementValueAt(pJsonParser, "web-app.servlet.servlet-name", 4));


    return 0;
}
