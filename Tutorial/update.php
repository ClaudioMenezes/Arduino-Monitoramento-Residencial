<?php
	require_once('controllers/session.php');
	// Check if the POST have all the necessary attributes
	if (!empty($_POST['id']) && !empty($_POST['typeAlert']) && !empty($_POST['location'])) {
		// Get the owner of the POST
		$userAux = $user->find($_POST['id']);
		if (($userAux != NULL)) {
			$alert = new Alert();
			$alertUserAux = $alert->getLastAlertUser($userAux['id']);
			$alert->id_user = $userAux['id'];
			$alert->type_alert = $_POST['typeAlert'];
			$alert->location = $_POST['location'];
			$alert->date_update = $now;
			
			if (($alertUserAux == NULL) || (($alertUserAux['type_alert'] != $_POST['typeAlert']) || ($alertUserAux['location'] != $_POST['location'])) && ($_POST['typeAlert'] == 'normal')) { // Just update the alert to "normal" situation
				$alert->create();
			} else if (($alertUserAux == NULL) || (($alertUserAux['type_alert'] != $_POST['typeAlert']) || ($alertUserAux['location'] != $_POST['location'])) && ($_POST['typeAlert'] != 'normal')) { // If new situation is different from the last one, create a new record and send a email to the owner
				$alert->create();
				$email = new Email();
				$email->send($userAux['email'], $_POST['typeAlert'] . '_' . $_POST['location']);
			} else { // Just update the situation
				$alert->save($alertUserAux['id']);
			}
		}
	}