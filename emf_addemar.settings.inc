<?php
/**
 * @file
 * Settings functions and callbacks.
 */

function emf_addemar_settings($form_state) {
  $form = array();
  
  use_soap_error_handler(false);

  // info
  $form['emf_addemar_token'] = array(
    '#type' => 'textfield',
    '#title' => t('Token'),
    '#description' => t('Your Addemar token.'),
    '#default_value' => variable_get('emf_addemar_token', ''),
    '#required' => TRUE,
  );

  $form['emf_addemar_server'] = array(
    '#type' => 'textfield',
    '#title' => t('Server'),
    '#description' => t('The Addemar server to connect to, by default this is ws.email.addemar.com'),
    '#default_value' => variable_get('emf_addemar_server', 'ws.email.addemar.com'),
    '#required' => TRUE, 
  );

  return system_settings_form($form);
}

