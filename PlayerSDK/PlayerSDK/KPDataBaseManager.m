//
//  KPDataBaseManager.m
//  KALTURAPlayerSDK
//
//  Created by Nissim Pardo on 8/3/15.
//  Copyright (c) 2015 Kaltura. All rights reserved.
//

#import "KPDataBaseManager.h"
#import "NSString+Utilities.h"
#import "KPLog.h"

static NSString *const CoreDataFileName = @"KPURLProtocolCaching";

#define MB (1024*1024)
#define GB (MB*1024)

@interface KPDataBaseManager()

@property (strong, nonatomic) NSManagedObjectModel *managedObjectModel;
@property (strong, nonatomic) NSPersistentStoreCoordinator *persistentStoreCoordinator;
@property (strong, nonatomic, readonly) NSBundle *bundle;
@property (strong, nonatomic, readonly) NSDictionary *cacheConditions;

@end

@implementation KPDataBaseManager
@synthesize bundle = _bundle, cacheConditions = _cacheConditions, withDomain = _withDomain, subStrings = _subStrings;

+ (KPDataBaseManager *)shared {
    static KPDataBaseManager *instance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[self alloc] init];
    });
    return instance;
}

// Returns the managed object context for the application.
// If the context doesn't already exist, it is created and bound to the persistent store coordinator for the application.
- (NSManagedObjectContext *)managedObjectContext {
    if (_managedObjectContext != nil) {
        return _managedObjectContext;
    }
    
    NSPersistentStoreCoordinator *coordinator = [self persistentStoreCoordinator];
    if (coordinator != nil) {
        _managedObjectContext = [[NSManagedObjectContext alloc] init];
        [_managedObjectContext setPersistentStoreCoordinator:coordinator];
    }
    return _managedObjectContext;
}

// Returns the managed object model for the application.
// If the model doesn't already exist, it is created from the application's model.
- (NSManagedObjectModel *)managedObjectModel {
    if (_managedObjectModel != nil) {
        return _managedObjectModel;
    }
    NSURL *modelURL = [self.bundle URLForResource:CoreDataFileName withExtension:@"momd"];
    _managedObjectModel = [[NSManagedObjectModel alloc] initWithContentsOfURL:modelURL];
    return _managedObjectModel;
}

// Returns the persistent store coordinator for the application.
// If the coordinator doesn't already exist, it is created and the application's store added to it.
- (NSPersistentStoreCoordinator *)persistentStoreCoordinator {
    if (_persistentStoreCoordinator != nil) {
        return _persistentStoreCoordinator;
    }
    
    NSURL *storeURL = [self.applicationDocumentsDirectory URLByAppendingPathComponent:CoreDataFileName.sqlite];
    NSError *error = nil;
    _persistentStoreCoordinator = [[NSPersistentStoreCoordinator alloc] initWithManagedObjectModel:[self managedObjectModel]];
    if (![_persistentStoreCoordinator addPersistentStoreWithType:NSSQLiteStoreType configuration:nil URL:storeURL options:nil error:&error]) {
        /*
         Replace this implementation with code to handle the error appropriately.
         
         abort() causes the application to generate a crash log and terminate. You should not use this function in a shipping application, although it may be useful during development.
         
         Typical reasons for an error here include:
         * The persistent store is not accessible;
         * The schema for the persistent store is incompatible with current managed object model.
         Check the error message to determine what the actual problem was.
         
         
         If the persistent store is not accessible, there is typically something wrong with the file path. Often, a file URL is pointing into the application's resources directory instead of a writeable directory.
         
         If you encounter schema incompatibility errors during development, you can reduce their frequency by:
         * Simply deleting the existing store:
         [[NSFileManager defaultManager] removeItemAtURL:storeURL error:nil]
         
         * Performing automatic lightweight migration by passing the following dictionary as the options parameter:
         @{NSMigratePersistentStoresAutomaticallyOption:@YES, NSInferMappingModelAutomaticallyOption:@YES}
         
         Lightweight migration will only work for a limited set of schema changes; consult "Core Data Model Versioning and Data Migration Programming Guide" for details.
         
         */
        NSLog(@"Unresolved error %@, %@", error, [error userInfo]);
        [[NSFileManager defaultManager] removeItemAtURL:storeURL error:nil];
    }
    return _persistentStoreCoordinator;
}


// Returns the URL to the application's Documents directory.
- (NSURL *)applicationDocumentsDirectory {
    return [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject];
}

// The SDK's bundle
- (NSBundle *)bundle {
    if (!_bundle) {
        _bundle = [NSBundle bundleWithURL:[[NSBundle mainBundle]
                                           URLForResource:@"KALTURAPlayerSDKResources"
                                           withExtension:@"bundle"]];
    }
    return _bundle;
}


// Fetches the White list urls
- (NSDictionary *)cacheConditions {
    if (!_cacheConditions) {
        NSString *path = [self.bundle pathForResource:@"CachedStrings" ofType:@"plist"];
        _cacheConditions = [NSDictionary dictionaryWithContentsOfFile:path];
    }
    return _cacheConditions;
}

// The url list which have to be checked by the domain first
- (NSDictionary *)withDomain {
    if (!_withDomain) {
        _withDomain = self.cacheConditions[@"withDomain"];
    }
    return _withDomain;
}


// The url list which should contain substring fron the White list
- (NSDictionary *)subStrings {
    if (!_subStrings) {
        _subStrings = self.cacheConditions[@"substrings"];
    }
    return _subStrings;
}

- (float)cachedSize {
    float size = 0;
    NSError *error = nil;
    NSFetchRequest *request = [NSFetchRequest fetchRequestWithEntityName:@"CachedURLResponse"];
    NSArray *matches = [dataBaseMgr.managedObjectContext executeFetchRequest:request error:&error];
    if (error) {
        KPLogError(@"Failed to Fetch from Core Data %@", error);
    }
    for (CachedURLResponse *response in matches) {
        size += response.data.length;
    }
    return size / MB;
}


@end

@implementation NSString (CoreData)


// Looks for cahced response with the current URL
- (CachedURLResponse *)cachedResponse {
    NSFetchRequest *request = [NSFetchRequest fetchRequestWithEntityName:@"CachedURLResponse"];
    request.predicate = [NSPredicate predicateWithFormat:@"url == %@", self];
    NSError *error = nil;
    NSArray *matches = [dataBaseMgr.managedObjectContext executeFetchRequest:request error:&error];
    if (error || !matches.count) {
        KPLogError(@"%@", error);
        return nil;
    }
    ((CachedURLResponse *)matches[0]).lastUsed = [NSDate date];
    return matches.lastObject;
}

@end

@implementation CachedURLParams

- (void)storeCacheResponse {
    float cachedSize = dataBaseMgr.cachedSize;
    
    // Checks the size of the cache and if erasing is needed then erase the less used urls
    if (cachedSize > self.freeDiskSpace || cachedSize > dataBaseMgr.cacheSize) {
        float overflowSize = cachedSize - dataBaseMgr.cacheSize + (float)self.data.length / MB;
        NSFetchRequest *request = [NSFetchRequest fetchRequestWithEntityName:@"CachedURLResponse"];
        
        request.sortDescriptors = @[[NSSortDescriptor sortDescriptorWithKey:@"lastUsed" ascending:YES]];
        NSArray *matches = [dataBaseMgr.managedObjectContext executeFetchRequest:request error:nil];
        
        for (CachedURLResponse *response in matches) {
            if (overflowSize > 0) {
                overflowSize -= (float)response.data.length / MB;
                [dataBaseMgr.managedObjectContext deleteObject:response];
            } else {
                break;
            }
        }
    }
    
    // Store the page
    CachedURLResponse *response = [NSEntityDescription insertNewObjectForEntityForName:@"CachedURLResponse"
                                                                inManagedObjectContext:dataBaseMgr.managedObjectContext];
    KPLogTrace(@"Cache URL: %@", self.url.absoluteString);
    response.data = self.data;
    response.url = self.url.absoluteString;
    response.timestamp = [NSDate date];
    response.mimeType = self.response.MIMEType;
    response.encoding = self.response.textEncodingName;
    response.lastUsed = [NSDate date];
    NSError *error = nil;
    [dataBaseMgr.managedObjectContext save:&error];
    if (error) {
        KPLogError(@"%@", error);
    }
}

- (NSMutableData *)data {
    if (!_data) {
        _data = [[NSMutableData alloc] init];
    }
    return _data;
}

- (long long)freeDiskSpace {
    return [[[[NSFileManager defaultManager] attributesOfFileSystemForPath:NSHomeDirectory() error:nil] objectForKey:NSFileSystemFreeSize] longLongValue];
}
@end