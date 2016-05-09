//
//  ChromecastHandler.h
//  KALTURAPlayerSDK
//
//  Created by Eliza Sapir on 04/05/2016.
//  Copyright © 2016 Kaltura. All rights reserved.
//

/**
 * @enum KPGCConnectionState
 * Enum defining GCKDeviceManager connection states.
 */
typedef NS_ENUM(NSInteger, KPGCConnectionState) {
    /** Disconnected from the device or application. */
    KPGCConnectionStateDisconnected = 0,
    /** Connecting to the device or application. */
    KPGCConnectionStateConnecting = 1,
    /** Connected to the device or application. */
    KPGCConnectionStateConnected = 2,
    /** Disconnecting from the device. */
    KPGCConnectionStateDisconnecting = 3
};

/**
 * @enum KPGCMediaStreamType
 * Enum defining the media stream type.
 */
typedef NS_ENUM(NSInteger, KPGCMediaStreamType) {
    /** A stream type of "none". */
    KPGCMediaStreamTypeNone = 0,
    /** A buffered stream type. */
    KPGCMediaStreamTypeBuffered = 1,
    /** A live stream type. */
    KPGCMediaStreamTypeLive = 2,
    /** An unknown stream type. */
    KPGCMediaStreamTypeUnknown = 99,
};

/**
 * @enum KPGCErrorCode
 * Description of error codes
 */
typedef NS_ENUM(NSInteger, KPGCErrorCode) {
    /**
     * Error Code indicating no error.
     */
    KPGCErrorCodeNoError = 0,
    
    /**
     * Error code indicating a network I/O error.
     */
    KPGCErrorCodeNetworkError = 1,
    
    /**
     * Error code indicating that an operation has timed out.
     */
    KPGCErrorCodeTimeout = 2,
    
    /**
     * Error code indicating an authentication error.
     */
    KPGCErrorCodeDeviceAuthenticationFailure = 3,
    
    /**
     * Error code indicating that an invalid request was made.
     */
    KPGCErrorCodeInvalidRequest = 4,
    
    /**
     * Error code indicating that an in-progress request has been cancelled, most likely because
     * another action has preempted it.
     */
    KPGCErrorCodeCancelled = 5,
    
    /**
     * Error code indicating that a request has been replaced by another request of the same type.
     */
    KPGCErrorCodeReplaced = 6,
    
    /**
     * Error code indicating that the request was disallowed and could not be completed.
     */
    KPGCErrorCodeNotAllowed = 7,
    
    /**
     * Error code indicating that a request could not be made because the same type of request is
     * still in process.
     */
    KPGCErrorCodeDuplicateRequest = 8,
    
    /**
     * Error code indicating that the request is not allowed in the current state.
     */
    KPGCErrorCodeInvalidState = 9,
    
    /**
     * Error code indicating that a requested application could not be found.
     */
    KPGCErrorCodeApplicationNotFound = 20,
    
    /**
     * Error code indicating that a requested application is not currently running.
     */
    KPGCErrorCodeApplicationNotRunning = 21,
    
    /**
     * Error code indicating that the application session ID was not valid.
     */
    KPGCErrorCodeInvalidApplicationSessionID = 22,
    
    /**
     * Error code indicating that a media load failed on the receiver side.
     */
    KPGCErrorCodeMediaLoadFailed = 30,
    
    /**
     * Error code indicating that a media media command failed because of the media player state.
     */
    KPGCErrorCodeInvalidMediaPlayerState = 31,
    
    /**
     * Error code indicating the app entered the background.
     */
    KPGCErrorCodeAppDidEnterBackground = 91,
    
    /**
     * Error code indicating a disconnection occurred during the request.
     */
    KPGCErrorCodeDisconnected = 92,
    
    /**
     * Error code indicating that an unknown, unexpected error has occurred.
     */
    KPGCErrorCodeUnknown = 99,
};


/**
 * @enum KPGCMediaPlayerIdleReason
 * Media player idle reasons.
 */
typedef NS_ENUM(NSInteger, KPGCMediaPlayerIdleReason) {
    /** Constant indicating that the player currently has no idle reason. */
    KPGCMediaPlayerIdleReasonNone = 0,
    
    /** Constant indicating that the player is idle because playback has finished. */
    KPGCMediaPlayerIdleReasonFinished = 1,
    
    /**
     * Constant indicating that the player is idle because playback has been cancelled in
     * response to a STOP command.
     */
    KPGCMediaPlayerIdleReasonCancelled = 2,
    
    /**
     * Constant indicating that the player is idle because playback has been interrupted by
     * a LOAD command.
     */
    KPGCMediaPlayerIdleReasonInterrupted = 3,
    
    /** Constant indicating that the player is idle because a playback error has occurred. */
    KPGCMediaPlayerIdleReasonError = 4,
};


typedef NS_ENUM(NSInteger, KPGCMediaPlayerState) {
    /** Constant indicating unknown player state. */
    KPGCMediaPlayerStateUnknown = 0,
    /** Constant indicating that the media player is idle. */
    KPGCMediaPlayerStateIdle = 1,
    /** Constant indicating that the media player is playing. */
    KPGCMediaPlayerStatePlaying = 2,
    /** Constant indicating that the media player is paused. */
    KPGCMediaPlayerStatePaused = 3,
    /** Constant indicating that the media player is buffering. */
    KPGCMediaPlayerStateBuffering = 4,
};


@protocol KPGCDevice <NSObject>
@property(nonatomic, copy) NSString *deviceID;
@property(nonatomic, copy) NSString *friendlyName;
@property(nonatomic, copy, readonly) NSString *ipAddress;
@property(nonatomic, readonly) UInt32 servicePort;
@end

@protocol KPGCMediaInformation;
@protocol KPGCMediaStatus <NSObject>
@property(nonatomic, readonly) KPGCMediaPlayerState playerState;
@property(nonatomic, readonly) KPGCMediaPlayerIdleReason idleReason;
@property(nonatomic, strong, readonly) id<KPGCMediaInformation> mediaInformation;
@end

@protocol KPGCMediaControlChannel <NSObject>
@property(nonatomic, strong, readonly) id<KPGCMediaStatus> mediaStatus;
@property(nonatomic, weak) id delegate;
- (NSTimeInterval)approximateStreamPosition;
- (NSInteger)seekToTimeInterval:(NSTimeInterval)position;
- (NSInteger)requestStatus;
- (NSInteger)loadMedia:(id<KPGCMediaInformation>)mediaInfo
              autoplay:(BOOL)autoplay
          playPosition:(NSTimeInterval)playPosition;
- (void)play;
- (void)pause;
@end

@protocol KPGCCastChannel <NSObject>


@end

@protocol KPGCDeviceManager <NSObject>
@property(nonatomic, readonly) id<KPGCDevice> device;
@property(nonatomic, weak) id delegate;
@property(nonatomic, readonly) KPGCConnectionState applicationConnectionState;
- (instancetype)initWithDevice:(id<KPGCDevice>)device clientPackageName:(NSString *)clientPackageName;
- (NSInteger)stopApplicationWithSessionID:(NSString *)sessionID;
- (void)connect;
- (void)disconnect;
- (NSInteger)launchApplication:(NSString *)applicationID;
- (BOOL)addChannel:(id<KPGCMediaControlChannel>)channel;
@end

@protocol KPGCDeviceScannerListener;

@protocol KPGCDeviceScanner <NSObject>
- (void)addListener:(id<KPGCDeviceScannerListener>)listener;
@property(nonatomic, readonly, copy) NSArray *devices;
- (void)startScan;
@end

@protocol KPGCMediaMetadata <NSObject>
- (NSString *)stringForKey:(NSString *)key;
@end

@protocol KPGCMediaInformation <NSObject>
@property(nonatomic, strong, readonly) id<KPGCMediaMetadata> metadata;
@property(nonatomic, readonly) NSTimeInterval streamDuration;
@property(nonatomic, copy, readonly) NSString *contentID;
@property(nonatomic, copy, readonly) NSString *contentType;
- (instancetype)initWithContentID:(NSString *)contentID
                       streamType:(KPGCMediaStreamType)streamType
                      contentType:(NSString *)contentType
                         metadata:(id<KPGCMediaMetadata>)metadata
                   streamDuration:(NSTimeInterval)streamDuration
                       customData:(id)customData;
@end



@protocol KPGCApplicationMetadata <NSObject>
@property(nonatomic, strong, readonly) id<KPGCMediaMetadata> metadata;
@end

@protocol KPGCError <NSObject>
@property (readonly) NSInteger code;
@end




@protocol KPGCDeviceScannerListener <NSObject>

@optional

/**
 * Called when a device has been discovered or has come online.
 *
 * @param device The device.
 */
- (void)deviceDidComeOnline:(id<KPGCDevice>)device;

/**
 * Called when a device has gone offline.
 *
 * @param device The device.
 */
- (void)deviceDidGoOffline:(id<KPGCDevice>)device;

/**
 * Called when there is a change to one or more properties of the device that do not affect
 * connectivity to the device. This includes all properties except the device ID, IP address,
 * and service port; if any of these properties changes, the device will be reported as "offline"
 * and a new device with the updated properties will be reported as "online".
 *
 * @param device The device.
 */
- (void)deviceDidChange:(id<KPGCDevice>)device;

@end