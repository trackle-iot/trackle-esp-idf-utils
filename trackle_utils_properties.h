#ifndef TRACKLE_UTILS_PROPERTIES_H
#define TRACKLE_UTILS_PROPERTIES_H

#include <stdbool.h>
#include <esp_types.h>
#include <esp_timer.h>

/**
 *
 * @file trackle_utils_properties.h
 * @brief Datatypes and functions for working with properties. Read the full description to learn about properties
 *
 * There are two concepts when it comes to working with properties:
 *  - Properties;
 *  - Properties groups.
 *
 * Properties are variables that are periodically published to the cloud with their name (key).
 *
 * Properties groups are groups of properties that are published with the same time period.
 *
 * Only properties that are inside a group are published to the cloud.
 *
 * Properties groups are created as follows:
 *  1. Declare a variable of type \ref Trackle_PropGroupID_t;
 *  2. Assign the result of \ref Trackle_PropGroup_create to this variable;
 *  3. Repeat the steps from 1 to 2 for all the properties groups that must be created;
 *
 * Then, in order to create properties, one must follow these steps:
 *  1. Declare a variable of type \ref Trackle_PropID_t;
 *  2. Assign the result of \ref Trackle_Prop_create or \ref Trackle_Prop_createString to this variable;
 *  3. Add the created property to one or more groups with \ref Trackle_PropGroup_addProp;
 *  4. Repeat the steps from 1 to 3 for all the properties that must be created;
 *  5. Call \ref Trackle_Props_startTask to start the properties task.
 *
 * Now, one can work with properties (update, read, etc.) by using the remaining functions exposed by this file.
 *
 */

/**
 * @brief Max number of characters allowed in properties name.
 */
#define TRACKLE_MAX_PROP_NAME_LENGTH 20

/**
 * @brief Max number of properties groups that can be created.
 */
#define TRACKLE_MAX_PROPGROUPS_NUM 10

/**
 * @brief Max number of properties that can be created.
 */
#define TRACKLE_MAX_PROPS_NUM 30

/**
 * @brief Value returned on error by functions returning \ref Trackle_PropGroupID_t
 */
#define Trackle_PropGroupID_ERROR -1

/**
 * @brief Value returned on error by functions returning \ref Trackle_PropID_t
 */
#define Trackle_PropID_ERROR -1

/**
 * @brief Type of the ID of a property group.
 */
typedef int Trackle_PropGroupID_t;

/**
 * @brief Type of the ID of a property.
 */
typedef int Trackle_PropID_t;

/**
 * @brief Create a new properties group, grouping properties that must be published with the same period.
 * @param periodMs Period for the publication of the properties belonging to the group [ms]
 * @param onlyIfChanged If true, the properties in the group will be published after a period only if they were changed since last publication.
 * @return ID associated with the new created properties group, or \ref Trackle_PropGroupID_ERROR on failure.
 */
Trackle_PropGroupID_t Trackle_PropGroup_create(uint32_t periodMs, bool onlyIfChanged);

/**
 * @brief Add a property to a group. Note that a single property can be added to more than one group by calling this function multiple times.
 * @param propId ID of the property to be added to the group.
 * @param propGroupId ID of the group where to add the specified property.
 * @return true if property was added successfully to the group, false otherwise.
 */
bool Trackle_PropGroup_addProp(Trackle_PropID_t propId, Trackle_PropGroupID_t propGroupId);

/**
 * @brief Create a new numeric property.
 * @param name Name/key to be assigned to the property.
 * @param scale Divider to be applied to values used to update the property (propValue = newValue / scale)
 * @param numDecimals Number of decimal digits to be used when publishing the property to the cloud. It's used only if \ref scale differs from 1 (otherwise the property is an integer and it doesn't make sense).
 * @param sign If true, the property is signed, otherwise it's unsigned. It's used only if \ref scale equals 1 (otherwise the property is a floating point number and is signed by default).
 * @return ID associated with the new created property, or \ref Trackle_PropID_ERROR on failure.
 */
Trackle_PropID_t Trackle_Prop_create(const char *name, uint16_t scale, uint8_t numDecimals, bool sign);

/**
 * @brief Create a new string property.
 * @param name Name/key to be assigned to the property.
 * @param maxLength Maximum length of the string that will be contained in the property.
 * @return ID associated with the new created property, or \ref Trackle_PropID_ERROR on failure.
 */
Trackle_PropID_t Trackle_Prop_createString(const char *name, int maxLength);

/**
 * @brief Update the value of a numeric property.
 * @param propID ID of the property to be updated.
 * @param newValue New value of the property.
 * @return true if update was successful, false otherwise.
 */
bool Trackle_Prop_update(Trackle_PropID_t propID, int newValue);

/**
 * @brief Update the value of a string property.
 * @param propID ID of the property to be updated.
 * @param newValue New value of the property.
 * @return true if update was successful, false otherwise.
 */
bool Trackle_Prop_updateString(Trackle_PropID_t propID, const char *newValue);

/**
 * @brief Set the abilitation of a property.
 * @param propID ID of the property.
 * @param isDisabled If true, the property is disabled, otherwise it's enabled.
 * @return true if setting was successful, false otherwise.
 */
bool Trackle_Prop_setDisabled(Trackle_PropID_t propID, bool isDisabled);

/**
 * @brief Set delay that must pass between last set of value and the publishing. A call to \ref Trackle_Prop_update within this delay resets the count.
 * @param propID ID of the property.
 * @param debounceDelayMs Milliseconds of the delay.
 * @return If true, debounce delay set successfully, else false.
 */
bool Trackle_Prop_setDebounceDelay(Trackle_PropID_t propID, uint32_t debounceDelayMs);

/**
 * @brief Get abilitation of a property.
 * @param propID ID of the property.
 * @return true if property is disabled, false otherwise.
 */
bool Trackle_Prop_isDisabled(Trackle_PropID_t propID);

/**
 * @brief Get key of a property.
 * @param propID ID of the property.
 * @return Pointer to the name/key of the property (empty string if \ref propID doesn't identify a valid property)
 */
const char *Trackle_Prop_getKey(Trackle_PropID_t propID);

/**
 * @brief Get value of a property.
 * @param propID ID of the property.
 * @return Value of the property (-1 if \ref propID doesn't identify a valid property)
 */
int32_t Trackle_Prop_getValue(Trackle_PropID_t propID);

/**
 * @brief Get value of a string property.
 * @param propID ID of the property.
 * @param retValue Char array that will contain a copy of the actual value of the property
 * @param retValueMaxLen Maximum length of the string that the function is allowed to store in the \ref retValue array (max is array's size-1, for null char).
 * @return True on success, false on errors
 */
bool Trackle_Prop_getStringValue(Trackle_PropID_t propID, char *retValue, int retValueMaxLen);

/**
 * @brief Get scale of a property.
 * @param propID ID of the property.
 * @return Scale of the property (0 if \ref propID doesn't identify a valid property)
 */
uint16_t Trackle_Prop_getScale(Trackle_PropID_t propID);

/**
 * @brief Get number of decimals of a property.
 * @param propID ID of the property.
 * @return Number of decimals of the property (0 if \ref propID doesn't identify a valid property)
 */
uint8_t Trackle_Prop_getNumberOfDecimals(Trackle_PropID_t propID);

/**
 * @brief Tells if a property is signed.
 * @param propID ID of the property.
 * @return True if property is signed, false otherwise. False also if property doesn't exist.
 */
bool Trackle_Prop_isSigned(Trackle_PropID_t propID);

/**
 * @brief Start the task that publishes periodically the properties contained in every property group.
 * @return true if task started successfully, false otherwise.
 */
bool Trackle_Props_startTask();

/**
 * @brief Get the number of the properties created so far.
 * @return Number of properties created.
 */
int Trackle_Props_getNumber();

/**
 * @brief Set dafault value and changed of a new property
 * @param value Default value of a property
 * @param changed Default changed value of a property
 */
void Trackle_Prop_setDefaults(int32_t value, bool changed);

#endif
