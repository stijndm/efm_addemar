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

  return system_settings_form($form);
}

