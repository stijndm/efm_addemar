<?php


/**
 * Subscribe a user to a mailgroups.
 *
 * @param $email
 *   String; E-mail address to subscribe
 * @param $fields
 *   Array; Array of custom field values. Key is field. Value is value for the field.
 * @param $lid
 *   String; List ID of the list to subscribe to.
 * @return
 *   Boolean; TRUE if user is subscribed. FALSE if not.
 */
function emf_addemar_api_subscribe($email, $fields, $mgid) {
  if (empty($fields)) {
    $fields = array('');
  }

  //get contact id based on email
  $cid = _emf_addemar_api_call('getContactId', array(0,$email,1));

  $cid = $cid[0];

  //if contact does not exist add new contact
  if (!$cid) {

    $contact_data = _emf_addemar_api_call('getContactStructure');

    foreach ($fields as $key => $field) {
      $contact_data->fields[$key]->value = $field;
    }
    $contact_data->fields[0]->value = $email;

    $cid = _emf_addemar_api_call('createContact', array($contact_data));

  }

  // do api call
  $result = _emf_addemar_api_call('subscribeContact', array($cid, $mgid));
  if (!$result) return FALSE;

  return TRUE;
}

/**
 * Unsubscribe a user from a list.
 *
 * @param $email
 *   String; E-mail address to subscribe
 * @param $lid
 *   String; List ID of the list to subscribe to.
 * @return
 *   Boolean; TRUE if user is subscribed. FALSE if not.
 */
function emf_addemar_api_unsubscribe($email, $mgid = NULL) {
  //get contact id base on email
  $cid = _emf_addemar_api_call('getContactId', array(0,$email,1));

  // do api call
  $result = _emf_addemar_api_call('unsubscribeContact', array($cid[0], $mgid));

  if (!$result) return FALSE;

  return TRUE;
}

/**
 * Fetch subscribed subscribers from API.
 *
 * @param $date
 *   Mixed; If a string, should be in the date() format of 'Y-m-d H:i:s', otherwise, a Unix timestamp.
 * @param $lid
 *   String; List ID
 * @return
 *   Array; List of subscriber lists.
 */
function emf_addemar_api_get_subscribers_subscribed($date = 0, $mgid = NULL) {
  // do api call
  $cids = _emf_addemar_api_call('getContactIdByMailgroup', array($mgid));
  if (!$cids) return FALSE;

  $result = array();
  foreach ($cids as $cid) {
    $contact = _emf_addemar_api_call('getContactData', array($cid));
    $result[] = $contact->fields[0]->value;
  }
  return $result;
}

/**
 * Fetch unsubscribed subscribers from API.
 *
 * @param $date
 *   Mixed; If a string, should be in the date() format of 'Y-m-d H:i:s', otherwise, a Unix timestamp.
 * @param $lid
 *   String; List ID
 * @return
 *   Array; List of subscriber lists.
 */
function emf_addemar_api_get_subscribers_unsubscribed($date = 0, $mgid = NULL) {
  // do api call
//  $cids = _emf_addemar_api_call('getUnsubscribers', array(6));
//
//  if (!$cids) return FALSE;
//
//  $result = array();
//  foreach ($cids as $cid) {
//    $contact = _emf_addemar_api_call('getContactData', array($cid));
//    $result[] = $contact->fields[0]->value;
//  }
//
//  watchdog('emf_addemar', 'prcoessing unsubscribers '.$mgid);
  return array();
}

/**
 * Fetch lists from API.
 *
 * @return
 *   Array; List of subscriber lists.
 */
function emf_addemar_api_get_lists() {
  $fields = array(
    'lid' => 'mgid',
    'name_api' => 'name',
  );

  $mgids = _emf_addemar_api_call('getMailgroupId', array());

  $lists = array();
  foreach ($mgids as $mgid) {
    $list = _emf_addemar_api_call('getMailgroupData', array($mgid));

    $lists[$mgid]->lid = $mgid;
    $lists[$mgid]->name_api = $list->name;
  }
  return $lists;
}

/**
 * Fetch custom fields for some list from API.
 *
 * @param $lid
 *   String; List ID of the list.
 * @return
 *   Array; List of custom fields.
 */
function emf_addemar_api_get_custom_fields($lid) {
  $field_types = array(
    '1' => 'text',
    '2' => 'file',
    '3' => 'select_one',
    '4' => 'increment',
  );

  $result = _emf_addemar_api_call('getFields', array());

  $fields = array();
  foreach ($result as $key => $field) {
    $fields[$field->fid]->key = $key;
    $fields[$field->fid]->name = $field->name;
    $fields[$field->fid]->type = $field_types[$field->type];
    $fields[$field->fid]->options = $field->default;
  }

  return $fields;
}

/**
 * Do API call.
 *
 * @param $method
 *   String; The API method to call.
 * @param $params
 *   Array; Parameters for the API call.
 * @return
 *   Array; API result array.
 */
function _emf_addemar_api_call($method, $params = array()) {
  static $addemar;

  // fetching api key
  $token = variable_get('emf_addemar_token', '');
  $server = variable_get('emf_addemar_server', 'ws.email.addemar.com');
 
  // if no api key is specified, return false
  if (empty($token)) return FALSE;


  //if no connection open a new connection
  if (!$addemar) {
    //build url
    $url = 'http://' . $server . '/soap/wsdl/?' . http_build_query(array('token' => $token, 'version' => '1.1', 't' => rand()));

    // do api call
    $addemar = new SoapClient($url, array('exceptions' => 0));
  }


  $result = call_user_func_array(array($addemar, $method), $params);

  // if api result code is not 'ok', return false and write log
  if (is_soap_fault($result)) {
    watchdog('emf_addemar', 'Code - '. $result->faultcode .', Message - '. $result->faultstring, array(), WATCHDOG_ERROR);
    return FALSE;
  }

  return $result;
}


/**
 * Convert a UNIX timestamp to a date Addemar wants to receive.
 *
 * @param $timestamp
 *   Integer; The UNIX timestamp to convert.
 * @retun
 *   String; The Date in Addemar format.
 */
function emf_addemar_api_unix_to_service_time($timestamp = 0) {
  if ($timestamp) {
    return date('Y-m-d H:i:s', $timestamp);
  }
  return 0;
}