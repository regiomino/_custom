<?php

/**
 * Implements hook_rules_action_info().
 */
function regiomino_user_rules_action_info() {
 return array(
   'regiomino_user_change_password' => array(
     'label' => t('Change user\'s password'),
     'parameter' => array(
       'account' => array(
         'type' => 'user',
         'label' => t('User'),
        ),
       'password' => array(
         'type' => 'text',
         'label' => t('Password'),
       ),
     ),
     'group' => t('Regiomino User'),
   ),
 );
}

function regiomino_user_change_password($account, $password) {
  // If we pass the password in the $edit array, it will be properly hashed
	//watchdog('pwdaction', '@password', array('@password' => $password));
  $edit['pass'] = $password;
  user_save($account, $edit);
}