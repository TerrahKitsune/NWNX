//Send a message though the talk channel
//Only oReceiver will see the message
int Messages_SendTalk( object oSender, object oReceiver, string sMessage );

//Send a message though the whisper channel
//Only oReceiver will see the message
int Messages_SendWhisper( object oSender, object oReceiver, string sMessage );

//Send a message though the party channel
//Only oReceiver will see the message
int Messages_SendParty( object oSender, object oReceiver, string sMessage );

//Send a message though the tell channel
//Only oReceiver will see the message
int Messages_SendTell( object oSender, object oReceiver, string sMessage );

//Send a message though the shout channel
//Only oReceiver will see the message
int Messages_SendShout( object oSender, object oReceiver, string sMessage );

//Send a message though the dm channel
//Only oReceiver will see the message
int Messages_SendDMChannel( object oSender, object oReceiver, string sMessage );

int Messages_SendTalk( object oSender, object oReceiver, string sMessage ){

    SetLocalString( oReceiver, "NWNX!MESSAGES!1", ObjectToString( oSender )+" "+sMessage );
    int nRet = StringToInt( GetLocalString( oReceiver, "NWNX!MESSAGES!1" ) );
    DeleteLocalString( oReceiver, "NWNX!MESSAGES!1" );
    return nRet;
}

int Messages_SendWhisper( object oSender, object oReceiver, string sMessage ){

    SetLocalString( oReceiver, "NWNX!MESSAGES!2", ObjectToString( oSender )+" "+sMessage );
    int nRet = StringToInt( GetLocalString( oReceiver, "NWNX!MESSAGES!2" ) );
    DeleteLocalString( oReceiver, "NWNX!MESSAGES!2" );
    return nRet;
}

int Messages_SendParty( object oSender, object oReceiver, string sMessage ){

    SetLocalString( oReceiver, "NWNX!MESSAGES!3", ObjectToString( oSender )+" "+sMessage );
    int nRet = StringToInt( GetLocalString( oReceiver, "NWNX!MESSAGES!3" ) );
    DeleteLocalString( oReceiver, "NWNX!MESSAGES!3" );
    return nRet;
}

int Messages_SendTell( object oSender, object oReceiver, string sMessage ){

    SetLocalString( oReceiver, "NWNX!MESSAGES!4", ObjectToString( oSender )+" "+sMessage );
    int nRet = StringToInt( GetLocalString( oReceiver, "NWNX!MESSAGES!4" ) );
    DeleteLocalString( oReceiver, "NWNX!MESSAGES!4" );
    return nRet;
}

int Messages_SendShout( object oSender, object oReceiver, string sMessage ){

    SetLocalString( oReceiver, "NWNX!MESSAGES!5", ObjectToString( oSender )+" "+sMessage );
    int nRet = StringToInt( GetLocalString( oReceiver, "NWNX!MESSAGES!5" ) );
    DeleteLocalString( oReceiver, "NWNX!MESSAGES!5" );
    return nRet;
}

int Messages_SendDMChannel( object oSender, object oReceiver, string sMessage ){

    SetLocalString( oReceiver, "NWNX!MESSAGES!6", ObjectToString( oSender )+" "+sMessage );
    int nRet = StringToInt( GetLocalString( oReceiver, "NWNX!MESSAGES!6" ) );
    DeleteLocalString( oReceiver, "NWNX!MESSAGES!6" );
    return nRet;
}
